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

#include "ScoutServer.h"
#include "ScoutTransport.h"
#include "ClientProtocolHandler.h"
#include "ScoutingDataModel.h"
#include "ImageManager.h"
#include <QObject>
#include <memory>
#include <list>

class SyncManager : public QObject
{
	Q_OBJECT

public:
	SyncManager(xero::scouting::datamodel::ImageManager &images, int team_number);
	virtual ~SyncManager();

	void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	void run();
	void createTransports();
	bool isBusy() const {
		return client_ != nullptr;
	}

signals:
	void logMessage(const QString& msg);
	void enableApp();
	void disableApp();
	void syncComplete();

private:
	void displayMessage(const QString& msg);
	void syncWithRemote(xero::scouting::transport::ScoutTransport* transport);
	void clientError(const QString& errmsg);
	void clientDisconnected();
	void complete();

private:
	//
	// The team number of the robotics team, used in transports to ID the correct central
	// machine to sync with
	//
	int team_number_;

	//
	// If non-null, the client we are sync'ing with
	//
	ClientProtocolHandler* client_;

	//
	// Any previous clients that needs to be cleaned up
	//
	std::list<ClientProtocolHandler*> previous_;

	//
	// The set of server types we support for sync'ing
	//
	std::list<std::shared_ptr<xero::scouting::transport::ScoutServer>> transport_servers_;

	//
	// The data model to sync
	//
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;

	//
	// The image manager for supplying images to clients that need them
	//
	xero::scouting::datamodel::ImageManager& images_;
};

