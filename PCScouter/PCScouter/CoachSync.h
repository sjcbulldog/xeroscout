#pragma once

#include "ImageManager.h"
#include "ScoutTransport.h"
#include "ClientServerProtocol.h"
#include "ScoutingDataModel.h"
#include <QObject>

class CoachSync : public QObject
{
	Q_OBJECT

public:
	CoachSync(xero::scouting::transport::ScoutTransport* transport, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::datamodel::ImageManager& images, bool debug);
	virtual ~CoachSync();

	void start();

signals:
	void syncComplete();
	void syncError(const QString& err);
	void displayLogMessage(const QString& msg);

private:
	void receivedJSON(uint32_t ptype, const QJsonDocument& doc);
	void protocolAbort(const QString& errmsg);
	void disconnected();
	void displayProtocolLogMessage(const QString& msg);

	void requestImage();
	void handleCoreData(const QJsonDocument& doc);
	void handleImage(const QJsonDocument& doc);
	void handleScoutingData(const QJsonDocument& doc);
	void handleZebraData(const QJsonDocument& doc);
	void handMatchDetailData(const QJsonDocument& doc);
	void handleZebraDataRequest(const QJsonDocument& doc);
	void handleMatchDetailDataRequest(const QJsonDocument& doc);
	void handleError(const QJsonDocument& doc);

private:
	QStringList needed_images_;
	xero::scouting::transport::ClientServerProtocol* protocol_;
	std::map<uint32_t, std::function<void(const QJsonDocument& doc)>> handlers_;
	xero::scouting::datamodel::ImageManager& images_;
	bool comp_type_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	bool debug_;
};

