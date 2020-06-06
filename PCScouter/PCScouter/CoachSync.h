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
	CoachSync(xero::scouting::transport::ScoutTransport* transport, xero::scouting::datamodel::ImageManager& images, bool debug);
	virtual ~CoachSync();

	void start();
	void run();

	void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) {
		dm_ = dm;
	}

	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dataModel() {
		return dm_;
	}

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
	void requestZebra();
	void requestMatchDetail();
	void doneAndWaiting();

	void handleCoreData(const QJsonDocument& doc);
	void handleImage(const QJsonDocument& doc);
	void handleScoutingData(const QJsonDocument& doc);
	void handleZebraData(const QJsonDocument& doc);
	void handMatchDetailData(const QJsonDocument& doc);
	void handleZebraDataRequest(const QJsonDocument& doc);
	void handleMatchDetailDataRequest(const QJsonDocument& doc);
	void handleError(const QJsonDocument& doc);
	void handleSyncDone(const QJsonDocument& doc);

private:
	QStringList needed_images_;
	QStringList needed_zebra_;
	QStringList needed_match_detail_;

	xero::scouting::transport::ClientServerProtocol* protocol_;
	std::map<uint32_t, std::function<void(const QJsonDocument& doc)>> handlers_;
	xero::scouting::datamodel::ImageManager& images_;
	bool comp_type_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	bool debug_;


};

