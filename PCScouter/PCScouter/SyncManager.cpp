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

#include "SyncManager.h"
#include "USBServer.h"
#include "TCPServer.h"
#include "BluetoothServer.h"
#include <QDebug>

using namespace xero::scouting::datamodel;
using namespace xero::scouting::transport;

SyncManager::SyncManager(ImageManager &images, int team_number): images_(images)
{
	team_number_ = team_number;
	client_ = nullptr;
}

SyncManager::~SyncManager()
{
}

void SyncManager::setDataModel(std::shared_ptr<ScoutingDataModel> dm)
{
	dm_ = dm;
}

void SyncManager::createTransports()
{
	std::stringstream messages;

	std::shared_ptr<ScoutServer> server;

	server = std::make_shared<USBServer>(this);
	if (server->init(messages))
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithRemote);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}

	server = std::make_shared<TcpServer>(this);
	if (server->init(messages))
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithRemote);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}

#ifdef _XERO_BLUETOOTH_CLIENT
	server = std::make_shared<BluetoothServer>(team_number_, this);
	if (server->init())
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithRemote);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}
#endif
}

void SyncManager::run()
{
	for (auto prev : previous_)
		delete prev;
	previous_.clear();

	for (auto trans : transport_servers_)
		trans->run();
}

void SyncManager::syncWithRemote(ScoutTransport* trans)
{
	if (client_ != nullptr)
	{
		emit logMessage("Sync requested from '" + trans->description() + "' - refused, sync in progress");
		trans->close();
		delete trans;
	}

	emit logMessage("Sync requested from '" + trans->description() + "' - accepted");
	emit disableApp();

	client_ = new ClientProtocolHandler(trans, images_, dm_, true);
	
	// These three signals end the session with the client
	connect(client_, &ClientProtocolHandler::errorMessage, this, &SyncManager::clientError);
	connect(client_, &ClientProtocolHandler::complete, this, &SyncManager::complete);
	connect(client_, &ClientProtocolHandler::clientDisconnected, this, &SyncManager::clientDisconnected);

	// These two signals convey information to the central about what is happening
	connect(client_, &ClientProtocolHandler::displayLogMessage, this, &SyncManager::displayMessage);

	client_->start();
}

void SyncManager::displayMessage(const QString& msg)
{
	emit logMessage(msg);
}

void SyncManager::clientError(const QString& errmsg)
{
	qDebug() << "SyncManager: clientError '" + errmsg + "' - client protocol id = " << client_->id();

	emit logMessage("Client connection error - " + errmsg);
	previous_.push_back(client_);
	client_ = nullptr;

	emit enableApp();
}

void SyncManager::clientDisconnected()
{
	qDebug() << "SyncManager: clientDisconnected - client protocol id = " << client_->id();

	emit logMessage("Client disconnected");
	previous_.push_back(client_);
	client_ = nullptr;

	emit enableApp();
}

void SyncManager::complete()
{
	if (client_ != nullptr)
	{
		qDebug() << "SyncManager: complete - client protocol id = " << client_->id();

		emit syncComplete();
		emit enableApp();

		previous_.push_back(client_);
		client_ = nullptr;
	}
	else
	{
		qDebug() << "SyncManager: complete - client already complete";
	}
}