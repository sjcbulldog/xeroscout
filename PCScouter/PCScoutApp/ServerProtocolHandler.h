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

#include "ScoutTransport.h"
#include "ScoutingDataModel.h"
#include "TabletIdentity.h"
#include "ClientServerProtocol.h"
#include "ImageManager.h"
#include <QObject>
#include <QJsonDocument>
#include <QTimer>
#include <memory>

class ServerProtocolHandler : public QObject
{
	Q_OBJECT

public:
	ServerProtocolHandler(const xero::scouting::datamodel::TabletIdentity &id, 
							xero::scouting::datamodel::ImageManager &images,
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
	void requestImage();

private:
	void handleUnxpectedPacket(const QJsonDocument& doc);
	void handleTabletList(const QJsonDocument& doc);
	void handleCoreData(const QJsonDocument& doc);
	void handleTabletData(const QJsonDocument& doc);
	void handleScoutingData(const QJsonDocument& doc);
	void handleScoutingDataReply(const QJsonDocument& doc);
	void handleErrorReply(const QJsonDocument& doc);
	void handleImage(const QJsonDocument& doc);

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

	QStringList needed_images_;

	xero::scouting::datamodel::ImageManager& images_;
};

