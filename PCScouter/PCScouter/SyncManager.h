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

