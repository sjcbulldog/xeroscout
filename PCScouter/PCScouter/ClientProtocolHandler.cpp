//
// Copyright 2020 by Jack W. (Butch) Griffin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#include "ClientProtocolHandler.h"
#include "ScoutDataJsonNames.h"
#include "TcpTransport.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QBuffer>
#include <QTextCodec>

using namespace xero::scouting::datamodel;
using namespace xero::scouting::transport;

int ClientProtocolHandler::master_id_ = 0;

ClientProtocolHandler::ClientProtocolHandler(ScoutTransport* s, xero::scouting::datamodel::ImageManager& images, std::shared_ptr<ScoutingDataModel> model, bool debug) : images_(images)
{
	data_model_ = model;
	client_ = new ClientServerProtocol(s, true, false, debug);
	debug_ = debug;
	comp_type_ = 0;
	id_ = master_id_++;

	connect(client_, &ClientServerProtocol::jsonReceived, this, &ClientProtocolHandler::receivedJSON);
	connect(client_, &ClientServerProtocol::errorDetected, this, &ClientProtocolHandler::protocolAbort);
	connect(client_, &ClientServerProtocol::disconnected, this, &ClientProtocolHandler::disconnected);
	connect(client_, &ClientServerProtocol::displayLogMessage, this, &ClientProtocolHandler::displayProtocolLogMessage);

	handlers_.insert(std::make_pair(ClientServerProtocol::TabletIDPacket, std::bind(&ClientProtocolHandler::handleTabletID, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::CoachIDPacket, std::bind(&ClientProtocolHandler::handleCoachID, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::DataModelScoutingPacket, std::bind(&ClientProtocolHandler::handleScoutingData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ErrorReply, std::bind(&ClientProtocolHandler::handleErrorReply, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestScoutingData, std::bind(&ClientProtocolHandler::handleScoutingRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::SyncDone, std::bind(&ClientProtocolHandler::handleSyncDone, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestImageData, std::bind(&ClientProtocolHandler::handleImageRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestMatchDetailData, std::bind(&ClientProtocolHandler::handleMatchDetailDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestZebraData, std::bind(&ClientProtocolHandler::handleZebraDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::CompleteButListening, std::bind(&ClientProtocolHandler::handleCompleteButListening, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideZebraData, std::bind(&ClientProtocolHandler::handleProvideZebraData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideMatchDetailData, std::bind(&ClientProtocolHandler::handleProvideMatchDetailData, this, std::placeholders::_1)));

	requested_match_detail_ = false;
	requested_zebra_ = false;

	client_->start();
}

ClientProtocolHandler::~ClientProtocolHandler()
{
	delete client_;
}

void ClientProtocolHandler::displayProtocolLogMessage(const QString& msg)
{
	emit displayLogMessage(msg);
}

void ClientProtocolHandler::protocolAbort(const QString &errmsg)
{
	emit errorMessage("protocol error - " + errmsg);
}

void ClientProtocolHandler::disconnected()
{
	emit complete();
}

void ClientProtocolHandler::handleUnxpectedPacket(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;

	obj[JsonMessageName] = "Unexpected packet type received";
	reply.setObject(obj);
	client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

	emit errorMessage("protocol error - unexpected packet received");
}

void ClientProtocolHandler::handleImageRequest(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		emit errorMessage("protocol error - TabletID packet should have contained JSON object");
		return;
	}

	obj = doc.object();
	if (!obj.contains(JsonImageName) || !obj.value(JsonImageName).isString())
	{
		emit errorMessage("protocol error - JSON image name is missing");
		return;
	}

	QString imname = obj.value(JsonImageName).toString();
	auto image = images_.get(imname);

	if (image == nullptr)
	{
		replyobj[JsonMessageName] = "image '" + imname + "' is not found";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}
	else
	{
		QByteArray ba;
		QBuffer buffer(&ba);
		image->save(&buffer, "PNG");

		QByteArray a = ba.toBase64();
		replyobj[JsonImageName] = imname;
		replyobj[JsonImageDataName] = QTextCodec::codecForMib(106)->toUnicode(a);
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ProvideImageData, reply, comp_type_);
	}
}

void ClientProtocolHandler::handleTabletID(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		emit errorMessage("protocol error - TabletID packet should have contained JSON object");
		return;
	}

	obj = doc.object();

	if (obj.contains(JsonUuidName) && !obj.contains(JsonNameName) && !obj.contains(JsonEvKeyName))
	{
		handleTabletIDReset(obj);
	}
	else if (obj.contains(JsonUuidName) && obj.contains(JsonNameName) && obj.contains(JsonEvKeyName))
	{
		handleTabletIDSync(obj);
	}
	else
	{
		replyobj[JsonMessageName] = "Invalid TabletID packet - internal error";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("protocol error - TabletID packet should have contained JSON object");
		return;
	}
}

void ClientProtocolHandler::handleTabletIDReset(const QJsonObject& obj)
{
	QJsonObject replyobj;
	QJsonDocument reply;

	if (!obj.contains(JsonUuidName))
	{
		emit errorMessage("protocol error - TabletID missing '" + QString(JsonUuidName) + "' field");
		return;
	}

	if (!obj.value(JsonUuidName).isString())
	{
		emit errorMessage("protocol error - ErrorReply field '" + QString(JsonUuidName) + "' is not a string");
		return;
	}

	QUuid uid = QUuid::fromString(obj.value(JsonUuidName).toString());
	if (uid.isNull())
	{
		replyobj[JsonMessageName] = "TabletID field '" + QString(JsonUuidName) + "' was not a valid UUID";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("invalid UUID field received from tablet");
		return;
	}

	if (data_model_ == nullptr) {

		replyobj[JsonMessageName] = "the Central Scouting Machine does not have data loaded";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Tablet tried to sync with no data loaded on the central machine");
		return;
	}

	identity_ = TabletIdentity(uid);

	//
	// We got a tablet identity packet with a UUID but without a tablet
	// name, send the name list down and let the tablet user choose a name
	//
	QStringList list;
	list.append(data_model_->matchTablets());
	list.append(data_model_->teamTablets());

	replyobj[JsonTabletListName] = QJsonArray::fromStringList(list);

	list.clear();
	for (const TabletIdentity& id : data_model_->registeredTablets())
		list.push_back(id.name());

	replyobj[JsonRegisteredTabletListName] = QJsonArray::fromStringList(list);
	replyobj[JsonEvKeyName] = data_model_->evkey();

	reply.setObject(replyobj);

	client_->sendJson(ClientServerProtocol::TabletSelectionList, reply, comp_type_);
}

void ClientProtocolHandler::handleTabletIDSync(const QJsonObject&obj)
{
	QJsonObject replyobj;
	QJsonDocument reply;

	if (data_model_ == nullptr) {

		replyobj[JsonMessageName] = "the Central Scouting Machine does not have data loaded";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Tablet tried to sync with no data loaded on the central machine");
		return;
	}

	if (!obj.contains(JsonNameName))
	{
		emit errorMessage("protocol error - Tablet Name missing '" + QString(JsonNameName) + "' field");
		return;
	}

	if (!obj.value(JsonNameName).isString())
	{
		emit errorMessage("protocol error - Tablet Name field '" + QString(JsonNameName) + "' is not a string");
		return;
	}

	if (!obj.contains(JsonUuidName))
	{
		emit errorMessage("protocol error - UUID missing '" + QString(JsonUuidName) + "' field");
		return;
	}

	if (!obj.value(JsonUuidName).isString())
	{
		emit errorMessage("protocol error - UUID field '" + QString(JsonUuidName) + "' is not a string");
		return;
	}

	if (!obj.contains(JsonEvKeyName))
	{
		emit errorMessage("protocol error - Event Key field missing '" + QString(JsonEvKeyName) + "' field");
		return;
	}

	if (!obj.value(JsonEvKeyName).isString())
	{
		emit errorMessage("protocol error - Event Key field field '" + QString(JsonEvKeyName) + "' is not a string");
		return;
	}

	if (!obj.value(JsonResetName).isBool() && !obj.value(JsonResetName).isBool())
	{
		emit errorMessage("protocol error - Reset Enabled field '" + QString(JsonResetName) + "' is not a boolean value");
		return;
	}

	if (obj.value(JsonEvKeyName).toString() != data_model_->evkey()) 
	{
		replyobj[JsonMessageName] = "The event active on the tablet does not match the event on the central machine.  Reset the tablet to sync with the current event.";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Tablet tried to sync with event that does not match the loaded event");
		return;
	}

	reset_ = obj.value(JsonResetName).toBool();

	QUuid uid = QUuid::fromString(obj.value(JsonUuidName).toString());
	if (uid.isNull())
	{
		replyobj[JsonMessageName] = "TabletID field '" + QString(JsonUuidName) + "' was not a valid UUID";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("invalid UUID field received from tablet");
		return;
	}

	if (data_model_ == nullptr) {

		replyobj[JsonMessageName] = "the Central Scouting Machine does not have data loaded";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Tablet tried to sync with no data loaded on the central server");
		return;
	}

	QString name = obj.value(JsonNameName).toString();
	assert(name.length() > 0);

	identity_ = TabletIdentity(name, uid);
	
	// See if this tablet is valid for the data model
	if (!data_model_->matchTablets().contains(name) && !data_model_->teamTablets().contains(name))
	{
		replyobj[JsonMessageName] = "Tablet '" + name + "' is not a valid tablet";

		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		return;
	}

	if (obj.contains(JsonCompressionName) && obj.value(JsonCompressionName).isDouble())
	{
		comp_type_ = obj.value(JsonCompressionName).toInt();
	}
	emit tabletAttached(identity_);
	emit displayLogMessage("Sending 'core' data to tablet ...");

	if (obj.contains(JsonCompressionName) && replyobj.value(JsonCompressionName).isDouble())
	{
		comp_type_ = obj.value(JsonCompressionName).toInt();
	}

	client_->sendJson(ClientServerProtocol::DataModelCorePacket, data_model_->generateCoreJSONDocument(false), comp_type_);
}

void ClientProtocolHandler::handleScoutingRequest(const QJsonDocument& doc)
{
	QJsonObject obj = doc.object();

	if (!obj.contains(JsonNameName))
	{
		emit errorMessage("protocol error - TabletID missing '" + QString(JsonNameName) + "' field");
		return;
	}

	if (!obj.value(JsonNameName).isString())
	{
		emit errorMessage("protocol error - ErrorReply field '" + QString(JsonNameName) + "' is not a string");
		return;
	}

	QString tablet = obj.value(JsonNameName).toString();
	client_->sendJson(ClientServerProtocol::DataModelScoutingPacket, data_model_->generateScoutingData(nullptr, tablet), comp_type_);
}

void ClientProtocolHandler::handleScoutingData(const QJsonDocument& doc)
{
	QString err;
	QJsonObject obj;
	QJsonDocument senddoc;
	TabletIdentity id;
	QStringList pits, matches;

	emit displayLogMessage("Receiving table scouting data ...");
	if (!data_model_->loadScoutingDataJSON(doc, id, err, pits, matches))
	{
		emit displayLogMessage("Error loading scouting data - " + err);
		obj[JsonStatusName] = "bad";
	}
	else
	{
		obj[JsonStatusName] = "good";
		obj[JsonPitsChangedListName] = QJsonArray::fromStringList(pits);
		obj[JsonMatchesChangedListName] = QJsonArray::fromStringList(matches);
	}

	senddoc.setObject(obj);

	emit displayLogMessage("Sending tablet acknowldgement message ...");
	client_->sendJson(ClientServerProtocol::ScoutingDataReply, senddoc, comp_type_);

	emit complete();
}

void ClientProtocolHandler::handleErrorReply(const QJsonDocument& doc)
{
	if (!doc.isObject())
	{
		emit errorMessage("protocol error - ErrorReply packet should have contained JSON object");
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonMessageName))
	{
		emit errorMessage("protocol error - ErrorReply missing '" + QString(JsonMessageName) + "' field");
		return;
	}

	if (!obj.value(JsonMessageName).isString())
	{
		emit errorMessage("protocol error - ErrorReply field '" + QString(JsonMessageName) + "' is not a string");
		return;
	}

	emit errorMessage("error returned from tablet - " + obj.value(JsonMessageName).toString());
}

void ClientProtocolHandler::handleSyncDone(const QJsonDocument &doc)
{
	emit complete();
}

void ClientProtocolHandler::handleCoachID(const QJsonDocument& doc)
{
	QJsonObject obj = doc.object();
	QJsonObject replyobj;
	QJsonDocument reply;

	if (obj.contains(JsonCompressionName) && obj.value(JsonCompressionName).isDouble())
	{
		comp_type_ = obj.value(JsonCompressionName).toInt();
	}

	emit displayLogMessage("Sending 'core' data to coaches laptop ...");

	if (obj.contains(JsonCompressionName) && replyobj.value(JsonCompressionName).isDouble())
	{
		comp_type_ = obj.value(JsonCompressionName).toInt();
	}

	client_->sendJson(ClientServerProtocol::DataModelCorePacket, data_model_->generateCoreJSONDocument(false), comp_type_);
}

void ClientProtocolHandler::handleMatchDetailDataRequest(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		replyobj[JsonMessageName] = "image data json was invalid, top level was not an object";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
	{
		replyobj[JsonMessageName] = "image data json was invalid, json was invalid";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonArray array = obj.value(JsonZebraDataName).toArray();
	QStringList keys;
	for (int i = 0; i < array.size(); i++)
	{
		if (array[i].isString())
			keys.push_back(array[i].toString());
	}

	reply = data_model_->generateMatchDetailData(keys);
	client_->sendJson(ClientServerProtocol::ProvideMatchDetailData, reply, comp_type_);
}

void ClientProtocolHandler::handleZebraDataRequest(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		replyobj[JsonMessageName] = "image data json was invalid, top level was not an object";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
	{
		replyobj[JsonMessageName] = "image data json was invalid, json was invalid";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonArray array = obj.value(JsonZebraDataName).toArray();
	QStringList keys;
	for (int i = 0; i < array.size(); i++)
	{
		if (array[i].isString())
			keys.push_back(array[i].toString());
	}

	reply = data_model_->generateZebraData(keys);
	client_->sendJson(ClientServerProtocol::ProvideZebraData, reply, comp_type_);
}

void ClientProtocolHandler::handleCompleteButListening(const QJsonDocument& doc)
{
	QJsonObject replyobj;
	QJsonDocument reply;
	QJsonArray zebra;

	//
	// Ask for missing zebra data, tell the central what matches are missing data
	//
	for (auto m : data_model_->matches())
	{
		if (m->hasZebra())
			zebra.push_back(m->key());
	}

	replyobj[JsonZebraDataName] = zebra;
	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::RequestZebraData, reply, comp_type_);
}

void ClientProtocolHandler::handleProvideZebraData(const QJsonDocument& doc)
{
	QJsonObject replyobj;
	QJsonDocument reply;

	if (!data_model_->loadZebraData(doc))
	{
		replyobj[JsonMessageName] = "image data json was invalid, could not load zebra data";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	//
	// Ask for missing match detail data, tell the central what matches are missing data
	//
	QJsonArray badata;
	for (auto m : data_model_->matches())
	{
		if (m->hasBlueAllianceData())
			badata.push_back(m->key());
	}

	replyobj[JsonZebraDataName] = badata;
	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::RequestZebraData, reply, comp_type_);
}

void ClientProtocolHandler::handleProvideMatchDetailData(const QJsonDocument& doc)
{
}

void ClientProtocolHandler::receivedJSON(uint32_t ptype, const QJsonDocument& doc)
{
	if (debug_)
		emit displayLogMessage("ClientProtocolHandler: received packet '" + ClientServerProtocol::pktTypeToString(ptype) + "'");

	auto it = handlers_.find(ptype);
	if (it != handlers_.end())
	{
		it->second(doc);
	}
	else {
		QString msg = "no handler registered for packet type 0x" + QString::number(ptype, 16);
		displayLogMessage(msg);
	}
}
