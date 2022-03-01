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

#pragma once

#include "TabletIdentity.h"
#include "ClientServerProtocol.h"
#include "ScoutingDataModel.h"
#include "ImageManager.h"
#include <QObject>
#include <QTcpSocket>
#include <QTextEdit>

class ClientProtocolHandler : public QObject
{
	Q_OBJECT

public:
	ClientProtocolHandler(xero::scouting::transport::ScoutTransport* socket, xero::scouting::datamodel::ImageManager& images,
			std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data, bool debug);
	virtual ~ClientProtocolHandler();

	void setDebug(bool b) { debug_ = b; }

	QString transportType() { return client_->transportType(); }

	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dataModel() {
		return data_model_;
	}

	int id() {
		return id_;
	}

	void start();

signals:
	void complete();
	void clientDisconnected();
	void tabletAttached(const xero::scouting::datamodel::TabletIdentity& name);
	void displayLogMessage(const QString& msg);
	void errorMessage(const QString& errmsg);

private:
	void receivedJSON(uint32_t ptype, const QJsonDocument& doc);
	void protocolAbort(const QString &errmsg);
	void disconnected();
	void displayProtocolLogMessage(const QString& msg);

	void requestZebraData();
	void requestMatchDetail();
	void coachComplete();

private:
	void handleUnxpectedPacket(const QJsonDocument& doc);
	void handleTabletID(const QJsonDocument& doc);
	void handleCoachID(const QJsonDocument& doc);
	void handleTabletIDReset(const QJsonObject& obj);
	void handleTabletIDSync(const QJsonObject& obj);
	void handleScoutingData(const QJsonDocument& doc);
	void handleErrorReply(const QJsonDocument& doc);
	void handleScoutingRequest(const QJsonDocument& doc);
	void handleSyncDone(const QJsonDocument& doc);
	void handleImageRequest(const QJsonDocument& doc);
	void handleNextImagePacketRequest(const QJsonDocument& doc);
	void handleMatchDetailDataRequest(const QJsonDocument& doc);
	void handleZebraDataRequest(const QJsonDocument& doc);
	void handleCompleteButListening(const QJsonDocument& doc);
	void handleProvideZebraData(const QJsonDocument& doc);
	void handleProvideMatchDetailData(const QJsonDocument& doc);
		
private:
	xero::scouting::transport::ClientServerProtocol* client_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data_model_;
	xero::scouting::datamodel::TabletIdentity identity_;
	bool debug_;
	int comp_type_;
	bool reset_;

	std::map<uint32_t, std::function<void(const QJsonDocument& doc)>> handlers_;

	xero::scouting::datamodel::ImageManager& images_;

	int id_;
	static int master_id_;

	bool requested_zebra_;
	bool requested_match_detail_;

	QStringList needed_zebra_;
	QStringList needed_match_detail_;

	QString image_name_;
	QString image_data_str_;
};

