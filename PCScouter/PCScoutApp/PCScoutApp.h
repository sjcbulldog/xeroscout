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

#include "ServerProtocolHandler.h"
#include "DocumentView.h"
#include "ScoutingDataModel.h"
#include "TabletIdentity.h"
#include "BluetoothClientTransport.h"
#include "BluetoothConnectDialog.h"
#include "FormInstance.h"
#include "ImageManager.h"
#include <QMainWindow>
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QFrame>
#include <QString>
#include <QTextEdit>
#include <QSettings>
#include <QIcon>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QUuid>
#include <QLabel>

class PCScoutApp : public QMainWindow
{
	Q_OBJECT

public:
	PCScoutApp(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent* ev);
	void showEvent(QShowEvent* ev);

private:
	void createMenus();
	void createWindows();

	QIcon loadIcon(const QString& filename);
	void listItemChanged(QListWidgetItem* olditem, QListWidgetItem* newitem);

	void resetTablet();
	void setTeamNumber();

	void syncWithCentralNetwork();
	void syncWithCentralUSB();
	void syncWithCentralNetworkDirect();
#ifdef BLUETOOTH_SYNC_NOT_READY
	void syncWithCentralBluetooth();
#endif
	void about();

	void readBroadcast();
	void streamConnected();
	void connectError(QAbstractSocket::SocketError err);

	void setMainView(xero::scouting::views::DocumentView::ViewType type);
	void toggleTeamsViewed(bool checked);

	void debugOutput(const QString& out);
	void setupViews();
	void setScoutingView(int index);
	bool viewExists(int index);

	void viewItemDoubleClicked(xero::scouting::views::DocumentView::ViewType type, const QString& key);
	int startScouting(const QString &key, const QString &type, const QString &title, xero::scouting::datamodel::Alliance c, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

	void extractDataFromForm(int formindex, const QString &label);

	void createTeamScoutingForms();
	void createMatchScoutingForms();
	void saveCurrentForm();
	void saveForm(int formindex);
	void saveAllForms();
	void saveAndBackup();

	void mergeDataFile();

	void removeAllScoutingFormsFromViews();

	QString generatePitTitle(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> t);
	QString generateMatchTitle(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> t);

	void startWaitForServer();

	void complete(bool reset);
	void displayLogMessage(const QString& msg);
	void errorMessage(const QString& error);
	void chooseTabletName(const QString &evkey, const QStringList& list, const QStringList &registered, QString& name);
	void processTimer();

	void startSync(xero::scouting::transport::ScoutTransport* trans);

	void debugToggled();

#ifdef BLUETOOTH_SYNC_NOT_READY
	void foundService(const QBluetoothDeviceInfo& info);
	void discoveryFinished();
	void serverSelected(const QString& name);
	void btConnected();
	void btConnectFailed();
#endif

private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windows";
	static constexpr const char* LeftRightSplitterSettings = "leftright";
	static constexpr const char* TopBottomSplitterSettings = "topbottom";
	static constexpr const char* CurrentEventKey = "eventkey";
	static constexpr const char* TabletNameKey = "tabletname";
	static constexpr const char* TabletGUIDKey = "tabletguid" ;
	static constexpr const char* ServerIPAddressName = "serverip";
	static constexpr const char* DebugParamName = "debug";

private:
	enum class State
	{
		Ready,
		WaitingForServer,
		WaitingForConnection,
		Synchronizing,
		Shutdown,
	};

private:
	QSplitter* left_right_splitter_;
	QSplitter* top_bottom_splitter_;
	QListWidget* view_selector_;
	xero::scouting::views::DocumentView* view_frame_;
	QWidget* database_view_;
	QTextEdit* logwin_;
	QLabel* tablet_name_;
	QLabel* uuid_display_;
	QLabel* data_model_display_;

	QTimer* timer_;

	//
	// Menu related items
	//
	QMenu* sync_menu_;
	QMenu* settings_menu_;
	QAction* debug_act_;
	QMenu* help_menu_;

	//
	// Application settings
	//
	QSettings settings_;

	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data_model_;

	xero::scouting::datamodel::TabletIdentity identity_;
	int team_number_;

	QUdpSocket* udp_socket_;
	QTcpSocket* tcp_socket_;
	QMetaObject::Connection tcp_connect_;
	QMetaObject::Connection tcp_error_;

	ServerProtocolHandler* server_;
	QMetaObject::Connection server_complete_connect_;
	QMetaObject::Connection server_display_connect_;
	QMetaObject::Connection server_error_connect_;
	QMetaObject::Connection server_choose_tablet_connect_;
	QMetaObject::Connection server_model_reset_connect_;

	State state_;

	bool ignore_view_select_changes_;

	QString server_ip_;

	xero::scouting::transport::BluetoothClientTransport* bt_transport_;
	BluetoothConnectDialog* dialog_;
	std::list<QBluetoothDeviceInfo> servers_;
	bool close_dialog_;

	bool host_addr_valid_;
	QHostAddress host_addr_;

	std::map<QString, std::shared_ptr<xero::scouting::datamodel::FormInstance>> form_instances_;

	xero::scouting::datamodel::ImageManager images_;
};
