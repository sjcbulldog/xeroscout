//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

