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

void SyncManager::createTransports()
{
	std::shared_ptr<ScoutServer> server;

	server = std::make_shared<USBServer>(this);
	if (server->init())
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithTablet);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}

	server = std::make_shared<TcpServer>(this);
	if (server->init())
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithTablet);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}

	server = std::make_shared<BluetoothServer>(team_number_, this);
	if (server->init())
	{
		(void)connect(server.get(), &ScoutServer::connected, this, &SyncManager::syncWithTablet);
		transport_servers_.push_back(server);
		emit logMessage("Synchronization transport '" + server->name() + "' initialized - " + server->hwinfo());
	}
	else
	{
		emit logMessage("Synchronization transport '" + server->name() + "' failed to initialized");
	}
}

void SyncManager::run(std::shared_ptr<ScoutingDataModel> dm)
{
	dm_ = dm;
	for (auto prev : previous_)
		delete prev;
	previous_.clear();

	for (auto trans : transport_servers_)
		trans->run();
}

void SyncManager::syncWithTablet(ScoutTransport* trans)
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
	qDebug() << "SyncManager: requested from '" << trans->description() << "' - created client protocol id = " << client_->id();

	// These three signals end the session with the client
	connect(client_, &ClientProtocolHandler::errorMessage, this, &SyncManager::clientError);
	connect(client_, &ClientProtocolHandler::complete, this, &SyncManager::complete);
	connect(client_, &ClientProtocolHandler::clientDisconnected, this, &SyncManager::clientDisconnected);

	// These two signals convey information to the central about what is happening
	connect(client_, &ClientProtocolHandler::displayLogMessage, this, &SyncManager::displayMessage);
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