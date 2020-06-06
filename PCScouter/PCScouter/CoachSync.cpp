#include "CoachSync.h"
#include "ScoutDataJsonNames.h"
#include <QJsonObject>
#include <QJsonArray>

using namespace xero::scouting::transport;
using namespace xero::scouting::datamodel;

CoachSync::CoachSync(ScoutTransport* transport, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, ImageManager &images, bool debug) : images_(images)
{
	comp_type_ = true;
	dm_ = dm;
	debug_ = debug;

	protocol_ = new ClientServerProtocol(transport, false, true, false);

	connect(protocol_, &ClientServerProtocol::jsonReceived, this, &CoachSync::receivedJSON);
	connect(protocol_, &ClientServerProtocol::errorDetected, this, &CoachSync::protocolAbort);
	connect(protocol_, &ClientServerProtocol::disconnected, this, &CoachSync::disconnected);
	connect(protocol_, &ClientServerProtocol::displayLogMessage, this, &CoachSync::displayProtocolLogMessage);

	handlers_.insert(std::make_pair(ClientServerProtocol::DataModelCorePacket, std::bind(&CoachSync::handleCoreData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideImageData, std::bind(&CoachSync::handleImage, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ScoutingDataReply, std::bind(&CoachSync::handleScoutingData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideZebraData, std::bind(&CoachSync::handleZebraData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ProvideMatchDetailData, std::bind(&CoachSync::handMatchDetailData, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestZebraData, std::bind(&CoachSync::handleZebraDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::RequestMatchDetailData, std::bind(&CoachSync::handleMatchDetailDataRequest, this, std::placeholders::_1)));
	handlers_.insert(std::make_pair(ClientServerProtocol::ErrorReply, std::bind(&CoachSync::handleError, this, std::placeholders::_1)));

	protocol_->start();
}

CoachSync::~CoachSync()
{
}

void CoachSync::run()
{
	protocol_->run();
}

void CoachSync::protocolAbort(const QString& errmsg)
{
	emit syncError("protocol aborted - " + errmsg);
}

void CoachSync::disconnected()
{
	emit syncComplete();
}

void CoachSync::displayProtocolLogMessage(const QString& msg)
{
	emit displayLogMessage(msg);
}

void CoachSync::start()
{
	QJsonDocument doc;
	QJsonObject obj;

	obj[JsonCompressionName] = comp_type_;
	doc.setObject(obj);
	protocol_->sendJson(ClientServerProtocol::CoachIDPacket, doc, comp_type_);
}

void CoachSync::requestImage()
{
	QJsonObject obj;
	QJsonDocument doc;

	if (needed_images_.count() > 0)
	{
		QString image = needed_images_.front();
		needed_images_.pop_front();

		obj[JsonImageName] = image;
		doc.setObject(obj);
		protocol_->sendJson(ClientServerProtocol::RequestImageData, doc, comp_type_);
	}
	else
	{
		doc.setObject(obj);
		obj[JsonNameName] = "*";
		protocol_->sendJson(ClientServerProtocol::RequestScoutingData, doc, comp_type_);
	}
}

void CoachSync::handleError(const QJsonDocument& doc)
{
	if (!doc.isObject())
	{
		emit displayLogMessage("protocol error - ErrorReply packet should have contained JSON object");
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonMessageName))
	{
		emit displayLogMessage("protocol error - ErrorReply missing '" + QString(JsonMessageName) + "' field");
		return;
	}

	if (!obj.value(JsonMessageName).isString())
	{
		emit displayLogMessage("protocol error - ErrorReply field '" + QString(JsonMessageName) + "' is not a string");
		return;
	}

	emit displayLogMessage("error returned from central laptop - " + obj.value(JsonMessageName).toString());
	emit syncError("Error From Central - " + obj.value(JsonMessageName).toString());
}

void CoachSync::handleImage(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject obj;

	if (!doc.isObject())
	{
		obj[JsonMessageName] = "image data json was invalid, json was not an object";
		reply.setObject(obj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	obj = doc.object();
	if (!obj.contains(JsonImageName) || !obj.value(JsonImageName).isString())
	{
		obj[JsonMessageName] = "image data json was invalid, missing 'image' field";
		reply.setObject(obj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	if (!obj.contains(JsonImageDataName) || !obj.value(JsonImageDataName).isString())
	{
		obj[JsonMessageName] = "image data json was invalid, missing 'imagedata' field";
		reply.setObject(obj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
	}

	QByteArray a = obj.value(JsonImageDataName).toString().toUtf8();
	QByteArray data = QByteArray::fromBase64(a);

	images_.put(obj.value(JsonImageName).toString(), data);

	requestImage();
}

void CoachSync::handleCoreData(const QJsonDocument& doc)
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

	if (!dm_->loadCoreJSON(doc, err))
	{
		//
		// Tell the central something went wrong
		//
		obj[JsonMessageName] = "error loading 'core' data on tablet - " + err.front();
		reply.setObject(obj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);

		//
		// Shutdown the connection
		//
		emit syncError("Error loading 'core' data - " + err.front());
	}
	else
	{
		QStringList imagelist = dm_->teamScoutingForm()->images();
		imagelist.append(dm_->matchScoutingForm()->images());

		for (const QString& image : imagelist)
		{
			if (images_.get(image) == nullptr)
				needed_images_.push_back(image);
		}

		requestImage();
	}
}

void CoachSync::handleScoutingData(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;
	QJsonObject obj;
	QJsonArray zebra;

	//
	// This will come back when we reset a tablet.  This is the core data and any scouting data
	// known by the host.
	//
	QStringList err;
	emit displayLogMessage("Received 'scouting' data from central machine");

	TabletIdentity tid;
	QString errstr;
	QStringList pits, matches;
	dm_->loadScoutingDataJSON(doc, tid, errstr, pits, matches);

	//
	// Ask for missing zebra data, tell the central what matches are missing data
	//
	for (auto m : dm_->matches())
	{
		if (m->hasZebra())
			zebra.push_back(m->key());
	}

	replyobj[JsonZebraDataName] = zebra;
	reply.setObject(replyobj);
	protocol_->sendJson(ClientServerProtocol::RequestZebraData, reply, comp_type_);
}

void CoachSync::handleZebraData(const QJsonDocument &doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;
	QJsonArray badata;

	//
	// Process zebra data
	//
	if (!dm_->loadZebraData(doc))
	{
		replyobj[JsonMessageName] = "image data json was invalid, could not load zebra data";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	//
	// Ask for missing match detail, tell the central what matches are missing data
	//
	for (auto m : dm_->matches())
	{
		if (m->hasBlueAllianceData())
			badata.push_back(m->key());
	}
	replyobj[JsonMatchesName] = badata;
	reply.setObject(replyobj);
	protocol_->sendJson(ClientServerProtocol::RequestMatchDetailData, reply, comp_type_);
}

void CoachSync::handMatchDetailData(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;

	//
	// Process the missing match data
	//
	if (!dm_->loadMatchDetailData(doc))
	{
		replyobj[JsonMessageName] = "image data json was invalid, could not load match detail data";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	//
	// Now tell the other end, we are complete but listening.  We will get a request for
	// zebra data, match detail data, or a complete message indicating we are done.
	//
	reply.setObject(replyobj);
	protocol_->sendJson(ClientServerProtocol::CompleteButListening, reply, comp_type_);
}

void CoachSync::handleZebraDataRequest(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		replyobj[JsonMessageName] = "image data json was invalid, top level was not an object";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
	{
		replyobj[JsonMessageName] = "image data json was invalid, json was invalid";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonArray array = obj.value(JsonZebraDataName).toArray();
	QStringList keys;
	for (int i = 0; i < array.size(); i++)
	{
		if (array[i].isString())
			keys.push_back(array[i].toString());
	}

	reply = dm_->generateZebraData(keys);
	protocol_->sendJson(ClientServerProtocol::ProvideZebraData, reply, comp_type_);
}

void CoachSync::handleMatchDetailDataRequest(const QJsonDocument& doc)
{
	QJsonDocument reply;
	QJsonObject replyobj;

	if (!doc.isObject())
	{
		replyobj[JsonMessageName] = "image data json was invalid, top level was not an object";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
	{
		replyobj[JsonMessageName] = "image data json was invalid, json was invalid";
		reply.setObject(replyobj);
		protocol_->sendJson(ClientServerProtocol::ErrorReply, reply, comp_type_);
		return;
	}

	QJsonArray array = obj.value(JsonZebraDataName).toArray();
	QStringList keys;
	for (int i = 0; i < array.size(); i++)
	{
		if (array[i].isString())
			keys.push_back(array[i].toString());
	}

	reply = dm_->generateMatchDetailData(keys);
	protocol_->sendJson(ClientServerProtocol::ProvideMatchDetailData, reply, comp_type_);
}

void CoachSync::receivedJSON(uint32_t ptype, const QJsonDocument& doc)
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