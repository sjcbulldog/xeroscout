//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
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
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestNextImageData, std::bind(&ClientProtocolHandler::handleNextImagePacketRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestMatchDetailData, std::bind(&ClientProtocolHandler::handleMatchDetailDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestZebraData, std::bind(&ClientProtocolHandler::handleZebraDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::CompleteButListening, std::bind(&ClientProtocolHandler::handleCompleteButListening, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideZebraData, std::bind(&ClientProtocolHandler::handleProvideZebraData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideMatchDetailData, std::bind(&ClientProtocolHandler::handleProvideMatchDetailData, this, std::placeholders::_1)));

	requested_match_detail_ = false;
	requested_zebra_ = false;
}

ClientProtocolHandler::~ClientProtocolHandler()
{
	delete client_;
}

void ClientProtocolHandler::start()
{
	client_->start();
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

void ClientProtocolHandler::handleNextImagePacketRequest(const QJsonDocument& doc)
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
	if (imname != image_name_) 
	{
		emit errorMessage("protocol error - JSON image name is mismatched between first packet and subsequent packets");
		return;
	}

	replyobj[JsonImageName] = imname;

	if (image_data_str_ > 16384) {
		replyobj[JsonImageDataName] = image_data_str_.left(16384);
		image_data_str_ = image_data_str_.mid(16384);
		replyobj[JsonImageDataStatus] = "more";
	}
	else {
		replyobj[JsonImageDataName];
		replyobj[JsonImageDataStatus] = "done";
	}

	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::ProvideImageData, reply, comp_type_);
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
	std::shared_ptr<QImage> image = images_.get(imname);

	if (image == nullptr)
	{
		replyobj[JsonMessageName] = "image '" + imname + "' is not found";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}
	else
	{
		QByteArray data;
		QBuffer buffer(&data);
		image->save(&buffer, "PNG");

		QByteArray b64 = data.toBase64();
		image_data_str_ = QTextCodec::codecForMib(106)->toUnicode(b64);
		image_name_ = imname;

		replyobj[JsonImageName] = imname;

		if (image_data_str_ > 16384) {
			replyobj[JsonImageDataName] = image_data_str_.left(16384);
			image_data_str_ = image_data_str_.mid(16384);
			replyobj[JsonImageDataStatus] = "more";
		}
		else {
			replyobj[JsonImageDataName];
			replyobj[JsonImageDataStatus] = "done";
		}

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

	if (obj.contains(JsonCompressionName) && obj.value(JsonCompressionName).isDouble())
	{
		comp_type_ = obj.value(JsonCompressionName).toInt();
	}

	client_->sendJson(ClientServerProtocol::DataModelCorePacket, data_model_->generateCoreJSONDocument(false), comp_type_);
}

void ClientProtocolHandler::handleScoutingRequest(const QJsonDocument& doc)
{
	QJsonObject obj = doc.object();
	auto keys = obj.keys();

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

	if (data_model_ == nullptr) {

		replyobj[JsonMessageName] = "the Central Scouting Machine does not have data loaded";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Central tried to sync with no data loaded on the central machine");
		return;
	}

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
		replyobj[JsonMessageName] = "match detail requeset, top level was not an object";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonMatchesDataName) || !obj.value(JsonMatchesDataName).isArray())
	{
		replyobj[JsonMessageName] = "match detail data json was invalid, json was invalid";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonArray array = obj.value(JsonMatchesDataName).toArray();
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
		replyobj[JsonMessageName] = "zebra data json was invalid, top level was not an object";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
	{
		replyobj[JsonMessageName] = "zebra data json was invalid, json was invalid";
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

	QJsonArray zebra;

	//
	// Ask for missing zebra data, tell the central what matches are missing data
	//
	needed_match_detail_.clear();
	needed_zebra_.clear();
	for (auto m : data_model_->matches())
	{
		if (!m->hasZebra())
			needed_zebra_.push_back(m->key());

		if (!m->hasBlueAllianceData())
			needed_match_detail_.push_back(m->key());
	}

	if (needed_zebra_.size() > 0)
		requestZebraData();
	else if (needed_match_detail_.size() > 0)
		requestMatchDetail();
	else
		coachComplete();
}

void ClientProtocolHandler::requestZebraData()
{
	QJsonObject replyobj;
	QJsonDocument reply;
	QJsonArray zebra;
	QString msg;

	while (zebra.size() < 10 && !needed_zebra_.isEmpty())
	{
		QString one = needed_zebra_.front();
		needed_zebra_.pop_front();

		if (msg.length() > 0)
			msg += ",";
		msg += one;

		zebra.push_back(one);
	}

	emit displayLogMessage("Central Requested Zebra: " + msg);

	replyobj[JsonZebraDataName] = zebra;
	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::RequestZebraData, reply, comp_type_);
}

void ClientProtocolHandler::requestMatchDetail()
{
	QJsonObject replyobj;
	QJsonDocument reply;
	QJsonArray matches;
	QString msg;

	while (matches.size() < 10 && !needed_match_detail_.isEmpty())
	{
		QString one = needed_match_detail_.front();
		needed_match_detail_.pop_front();

		if (msg.length() > 0)
			msg += ",";
		msg += one;

		matches.push_back(one);
	}

	emit displayLogMessage("Central Requested MatchDetail: " + msg);

	replyobj[JsonMatchesDataName] = matches;
	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::RequestMatchDetailData, reply, comp_type_);
}

void ClientProtocolHandler::coachComplete()
{
	QJsonObject replyobj;
	QJsonDocument reply;

	reply.setObject(replyobj);
	client_->sendJson(ClientServerProtocol::SyncDone, reply, comp_type_);

	emit complete();
}

void ClientProtocolHandler::handleProvideZebraData(const QJsonDocument& doc)
{
	QJsonObject replyobj;
	QJsonDocument reply;

	data_model_->blockSignals(true);
	if (!data_model_->loadZebraData(doc))
	{
		replyobj[JsonMessageName] = "zebra list was invalid, could not load zebra data";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		data_model_->blockSignals(false);
		return;
	}
	data_model_->blockSignals(false);

	if (needed_zebra_.size() > 0)
		requestZebraData();
	else if (needed_match_detail_.size() > 0)
		requestMatchDetail();
	else
		coachComplete();
}

void ClientProtocolHandler::handleProvideMatchDetailData(const QJsonDocument& doc)
{
	QJsonObject replyobj;
	QJsonDocument reply;

	data_model_->blockSignals(true);
	if (!data_model_->loadMatchDetailData(doc))
	{
		replyobj[JsonMessageName] = "match detail list was invalid, could not load match detail data";
		reply.setObject(replyobj);
		client_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		data_model_->blockSignals(false);
		return;
	}
	data_model_->blockSignals(false);

	if (needed_zebra_.size() > 0)
		requestZebraData();
	else if (needed_match_detail_.size() > 0)
		requestMatchDetail();
	else
		coachComplete();
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
