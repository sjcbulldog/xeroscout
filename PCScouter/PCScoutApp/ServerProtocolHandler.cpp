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

#include "ServerProtocolHandler.h"
#include "ScoutDataJsonNames.h"
#include "ClientServerProtocol.h"
#include <functional>

using namespace xero::scouting::datamodel;
using namespace xero::scouting::transport;

ServerProtocolHandler::ServerProtocolHandler(const TabletIdentity& id, xero::scouting::datamodel::ImageManager& images, 
	std::shared_ptr<ScoutingDataModel> dm, ScoutTransport* trans, int comptype, bool debug) : images_(images)
{
	done_ = false;
	comp_type_ = comptype;
	data_model_ = dm;
	debug_ = debug;
	id_ = id;
	tablet_name_assigned_ = false;

	handlers_.insert(std::make_pair(ClientServerProtocol::TabletIDPacket, std::bind(&ServerProtocolHandler::handleUnxpectedPacket, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::DataModelCorePacket, std::bind(&ServerProtocolHandler::handleCoreData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::DataModelScoutingPacket, std::bind(&ServerProtocolHandler::handleScoutingData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ScoutingDataReply, std::bind(&ServerProtocolHandler::handleScoutingDataReply, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::TabletSelectionList, std::bind(&ServerProtocolHandler::handleTabletList, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ErrorReply, std::bind(&ServerProtocolHandler::handleErrorReply, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideImageData, std::bind(&ServerProtocolHandler::handleImage, this, std::placeholders::_1)));

	server_ = new ClientServerProtocol(trans, false, true, debug_);
	connect(server_, &ClientServerProtocol::jsonReceived, this, &ServerProtocolHandler::jsonReceived);
	connect(server_, &ClientServerProtocol::errorDetected, this, &ServerProtocolHandler::protocolAbort);
	connect(server_, &ClientServerProtocol::disconnected, this, &ServerProtocolHandler::disconnected);
	connect(server_, &ClientServerProtocol::displayLogMessage, this, &ServerProtocolHandler::displayProtcolLogMessage);
}

ServerProtocolHandler::~ServerProtocolHandler()
{
	server_->shutdown();
	delete server_;
}

void ServerProtocolHandler::displayProtcolLogMessage(const QString& msg)
{
	emit displayLogMessage(msg);
}

void ServerProtocolHandler::protocolAbort(const QString &errmsg)
{
	emit errorMessage("protocol error - " + errmsg);
}

void ServerProtocolHandler::disconnected()
{
	if (!done_)
		emit errorMessage("central machine disconnected unexpectedly");
}

void ServerProtocolHandler::startSync()
{
	QJsonDocument doc;
	QJsonObject pkt;

	server_->start();

	//
	// This is the common sync scouting data case.
	//
	if (id_.name() == 0 || data_model_->evkey().length() == 0)
	{
		pkt[JsonUuidName] = id_.uid().toString();
		pkt[JsonCompressionName] = comp_type_;
		reset_ = true;
	}
	else
	{
		pkt[JsonNameName] = id_.name();
		pkt[JsonUuidName] = id_.uid().toString();
		pkt[JsonEvKeyName] = data_model_->evkey();
		pkt[JsonCompressionName] = comp_type_;
		pkt[JsonResetName] = false;
		reset_ = false;
	}

	doc.setObject(pkt);

	if (debug_)
		emit displayLogMessage("ServerProtocolHandler: starting sync of scouting data (sending TabletID packet)");

	server_->sendJson(ClientServerProtocol::TabletIDPacket, doc, 0);
}

void ServerProtocolHandler::run()
{
	server_->run();
}

void ServerProtocolHandler::handleUnxpectedPacket(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;

	obj[JsonMessageName] = "Unexpected packet type received";
	reply.setObject(obj);
	server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

	emit errorMessage("protocol error - unexpected packet received");
}

void ServerProtocolHandler::handleErrorReply(const QJsonDocument& doc)
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

	emit errorMessage("Error - " + obj.value(JsonMessageName).toString());
}

void ServerProtocolHandler::handleTabletList(const QJsonDocument& doc)
{
	QStringList list;
	QStringList registered;
	QString name;
	QJsonDocument reply;
	QJsonObject pkt;

	assert(reset_ == true);

	if (!doc.isObject())
	{
		emit errorMessage("protocol error - TabletList packet should have contained JSON object");
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonEvKeyName))
	{
		emit errorMessage("protocol error - Event Key missing '" + QString(JsonEvKeyName) + "' field");
		return;
	}

	if (!obj.value(JsonEvKeyName).isString())
	{
		emit errorMessage("protocol error - TabletList field '" + QString(JsonEvKeyName) + "' is not a string");
		return;
	}

	QString evkey = obj.value(JsonEvKeyName).toString();

	if (!obj.contains(JsonTabletListName))
	{
		emit errorMessage("protocol error - TabletList missing '" + QString(JsonTabletListName) + "' field");
		return;
	}

	if (!obj.value(JsonTabletListName).isArray())
	{
		emit errorMessage("protocol error - TabletList field '" + QString(JsonTabletListName) + "' is not a string list");
		return;
	}

	if (!obj.contains(JsonRegisteredTabletListName))
	{
		emit errorMessage("protocol error - TabletList missing '" + QString(JsonRegisteredTabletListName) + "' field");
		return;
	}

	if (!obj.value(JsonRegisteredTabletListName).isArray())
	{
		emit errorMessage("protocol error - TabletList field '" + QString(JsonRegisteredTabletListName) + "' is not a string list");
		return;
	}

	QJsonArray arr = obj.value(JsonTabletListName).toArray();
	for (int i = 0; i < arr.size(); i++)
	{
		if (!arr[i].isString())
		{
			emit errorMessage("protocol error - TabletList field '" + QString(JsonTabletListName) + "' contained a non-string value");
			return;
		}
		list.push_back(arr[i].toString());
	}

	arr = obj.value(JsonRegisteredTabletListName).toArray();
	for (int i = 0; i < arr.size(); i++)
	{
		if (!arr[i].isString())
		{
			emit errorMessage("protocol error - TabletList field '" + QString(JsonRegisteredTabletListName) + "' contained a non-string value");
			return;
		}
		registered.push_back(arr[i].toString());
	}

	emit chooseTabletName(evkey, list, registered, name);

	if (name.length() > 0)
	{
		id_ = TabletIdentity(name, id_.uid());
		tablet_name_assigned_ = true;

		pkt[JsonNameName] = id_.name();
		pkt[JsonUuidName] = id_.uid().toString();
		pkt[JsonEvKeyName] = evkey;
		pkt[JsonResetName] = true;
		pkt[JsonCompressionName] = comp_type_;
		reply.setObject(pkt);

		server_->sendJson(ClientServerProtocol::TabletIDPacket, reply, comp_type_);
	}
	else
	{
		pkt[JsonMessageName] = "Tablet selection aborted";
		reply.setObject(pkt);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		emit errorMessage("Tablet name not choosen - synchronization aborted");
	}
}

void ServerProtocolHandler::requestNextImagePart(const QString& imname)
{
	QJsonObject obj;
	QJsonDocument doc;

	obj[JsonImageName] = imname;
	doc.setObject(obj);
	server_->sendJson(ClientServerProtocol::RequestNextImageData, doc, comp_type_);
}

void ServerProtocolHandler::requestImage()
{
	QJsonObject obj;
	QJsonDocument doc;

	if (needed_images_.count() > 0)
	{
		QString image = needed_images_.front();
		needed_images_.pop_front();

		obj[JsonImageName] = image;
		doc.setObject(obj);
		server_->sendJson(ClientServerProtocol::RequestImageData, doc, comp_type_);
	}
	else if (!reset_)
	{
		emit displayLogMessage("Sending scouting data to central machine");

		//
		// Generate the scouting data for this tablet only (by name)
		//
		QJsonDocument scoutingData = data_model_->generateScoutingData(&id_, id_.name());

		//
		// Send the data down to the central
		//
		server_->sendJson(ClientServerProtocol::DataModelScoutingPacket, scoutingData, comp_type_);
	}
	else
	{
		//
		// We are doing a reset of the tablet, request any scouting data available
		//
		obj[JsonNameName] = id_.name();
		doc.setObject(obj);
		server_->sendJson(ClientServerProtocol::RequestScoutingData, doc, comp_type_);
	}
}

void ServerProtocolHandler::handleImage(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;

	if (!doc.isObject())
	{
		obj[JsonMessageName] = "image data json was invalid, json was not an object";
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	obj = doc.object();
	if (!obj.contains(JsonImageName) || !obj.value(JsonImageName).isString())
	{
		obj[JsonMessageName] = "image data json was invalid, missing 'image' field";
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	if (!obj.contains(JsonImageDataName) || !obj.value(JsonImageDataName).isString())
	{
		obj[JsonMessageName] = "image data json was invalid, missing 'imagedata' field";
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	QByteArray a = obj.value(JsonImageDataName).toString().toUtf8();
	QByteArray data = QByteArray::fromBase64(a);

	current_image_.append(data);
	if (obj.contains(JsonImageDataStatus) && obj.value(JsonImageDataStatus).isString() && obj.value(JsonImageDataStatus).toString() == "done") {
		images_.put(obj.value(JsonImageName).toString(), current_image_);
		requestImage();

		current_image_.clear();
	}
	else {
		requestNextImagePart(obj.value(JsonImageName).toString());
	}
}

void ServerProtocolHandler::handleCoreData(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;

	//
	// This will come back when we ask to sync the scouting data.  It just
	// ensures that the core data is up to date.  It can change throughout
	// the match and if it does, the data here on this tablet gets updated.
	//

	QStringList err;
	emit displayLogMessage("Received 'core' data from central machine - updating data model");

	QUuid uid;
	if (!data_model_->peekUUID(doc, uid))
	{
		//
		// Shutdown the connection
		//
		emit errorMessage("Error loading 'core' data - missing data model UUID");

		//
		// Tell the central something went wrong
		//
		obj[JsonMessageName] = "error loading 'core' data on tablet - missing data model UUID";
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		return;
	}

	if (!data_model_->uuid().isNull() && data_model_->uuid() != uid)
	{
		//
		// The data loaded locally is for a different data model, we cannot mix the data so
		// tell the user something is wrong
		//

		emit errorMessage("The data on this tablet is for a different data set, to sync this tablet with the given central, first reset the tablet, then sych.");

		//
		// Tell the central something went wrong
		//
		obj[JsonMessageName] = "data on tablet is for different dataset";
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		return;
	}

	if (!data_model_->loadCoreJSON(doc, err))
	{
		//
		// Shutdown the connection
		//
		emit errorMessage("Error loading 'core' data - " + err.front());

		//
		// Tell the central something went wrong
		//
		obj[JsonMessageName] = "error loading 'core' data on tablet - " + err.front();
		reply.setObject(obj);
		server_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}
	else
	{
		QStringList imagelist = data_model_->teamScoutingForm()->images();
		imagelist.append(data_model_->matchScoutingForm()->images());

		for (const QString& image : imagelist)
		{
			if (images_.get(image) == nullptr)
				needed_images_.push_back(image);
		}

		requestImage();
	}
}

void ServerProtocolHandler::handleScoutingData(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;
	QJsonObject obj;

	QString docstr = doc.toJson(QJsonDocument::Indented);

	TabletIdentity tid(id_);

	//
	// This will come back when we reset a tablet.  This is the core data and any scouting data
	// known by the host.
	//
	QStringList err;
	emit displayLogMessage("Received 'scouting' data from central machine");

	QString errstr;
	QStringList pits, matches;
	data_model_->loadScoutingDataJSON(doc, tid, errstr, pits, matches);

	reply.setObject(replyobj);
	server_->sendJson(ClientServerProtocol::SyncDone, reply, comp_type_);

	done_ = true;
	emit complete(true);
}

void ServerProtocolHandler::handleScoutingDataReply(const QJsonDocument& doc)
{
	if (!doc.isObject())
	{
		emit errorMessage("protocol error - ScoutingDataReply packet should have contained JSON object");
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonStatusName))
	{
		emit errorMessage("protocol error - ScoutingDataReply missing '" + QString(JsonStatusName) + "' field");
		return;
	}

	if (!obj.value(JsonStatusName).isString())
	{
		emit errorMessage("protocol error - ScoutingDataReply field '" + QString(JsonStatusName) + "' is not a string");
		return;
	}

	if (obj.value(JsonStatusName).toString() == "good")
	{
		if (obj.contains(JsonPitsChangedListName) && obj.value(JsonPitsChangedListName).isArray())
		{
			QJsonArray a = obj.value(JsonPitsChangedListName).toArray();
			if (a.size() > 0) {
				QString msg = "Updated Pits:";
				for (int i = 0; i < a.size(); i++)
				{
					if (a[i].isString())
					{
						if (i != 0)
							msg += ", ";
						msg += " " + a[i].toString();
					}
				}
				emit displayLogMessage(msg);
			}
		}

		if (obj.contains(JsonMatchesChangedListName) && obj.value(JsonMatchesChangedListName).isArray())
		{
			QJsonArray a = obj.value(JsonMatchesChangedListName).toArray();
			if (a.size() > 0) {
				QString msg = "Updated Matches:";
				for (int i = 0; i < a.size(); i++)
				{
					if (a[i].isString())
					{
						if (i != 0)
							msg += ", ";
						msg += " " + a[i].toString();
					}
				}
				emit displayLogMessage(msg);
			}
		}

		done_ = true;
		emit complete(false);
	}
	else
	{
		if (!obj.contains(JsonMessageName))
		{
			emit errorMessage("protocol error - ScoutingDataReply missing '" + QString(JsonMessageName) + "' field");
			return;
		}

		if (!obj.value(JsonMessageName).isString())
		{
			emit errorMessage("protocol error - ScoutingDataReply field '" + QString(JsonMessageName) + "' is not a string");
			return;
		}

		emit errorMessage("error returned by central - " + obj.value(JsonMessageName).toString());
	}
}

void ServerProtocolHandler::jsonReceived(uint32_t ptype, const QJsonDocument& doc)
{
	if (debug_)
		emit displayLogMessage("ServerProtocolHandler: recevied packet '" + ClientServerProtocol::pktTypeToString(ptype) + "'");

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
