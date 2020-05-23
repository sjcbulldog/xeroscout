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

#pragma once

#include "ScoutTransport.h"
#include "ScoutingDataModel.h"
#include "TabletIdentity.h"
#include "ClientServerProtocol.h"
#include <QObject>
#include <QJsonDocument>
#include <QTimer>
#include <memory>

class ServerProtocolHandler : public QObject
{
	Q_OBJECT

public:
	ServerProtocolHandler(const xero::scouting::datamodel::TabletIdentity &id, 
							std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, 
							xero::scouting::transport::ScoutTransport* trans, int comptype, bool debug=false);
	virtual ~ServerProtocolHandler();

	const xero::scouting::datamodel::TabletIdentity& identity() {
		return id_;
	}

	bool wasTabletNameAssigned() {
		return tablet_name_assigned_;
	}

	void run();

signals:
	void complete(bool reset);
	void displayLogMessage(const QString& msg);
	void errorMessage(const QString& error);
	void chooseTabletName(const QString &evkey, const QStringList& list, const QStringList &registered, QString& name);

private:
	void startSync();
	void jsonReceived(uint32_t ptype, const QJsonDocument& doc);
	void protocolAbort(const QString &errmsg);
	void disconnected();
	void displayProtcolLogMessage(const QString& msg);

private:
	void handleUnxpectedPacket(const QJsonDocument& doc);
	void handleTabletList(const QJsonDocument& doc);
	void handleCoreData(const QJsonDocument& doc);
	void handleTabletData(const QJsonDocument& doc);
	void handleScoutingData(const QJsonDocument& doc);
	void handleScoutingDataReply(const QJsonDocument& doc);
	void handleErrorReply(const QJsonDocument& doc);

private:
	//
	// The identity of the tablet, may be changed during the sync process
	//
	xero::scouting::datamodel::TabletIdentity id_;

	//
	// If true, the tablet name part of the identity was changed during the sync
	//
	bool tablet_name_assigned_;

	//
	// If true, spew debug information to help debug the synchronization process
	//
	bool debug_;

	//
	// The data model from the application that synchronizing will update
	//
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data_model_;

	//
	// The low level JSON client server protocol
	//
	xero::scouting::transport::ClientServerProtocol* server_;

	//
	// The mapping from packet type to method to handle the packet
	//
	std::map<uint32_t, std::function<void(const QJsonDocument& doc)>> handlers_;

	//
	// The type of compression
	//
	int comp_type_;

	//
	// If true, we are doing a reset on the tablet
	bool reset_;

	//
	// We are done, disconnects are ok
	//
	bool done_;
};

