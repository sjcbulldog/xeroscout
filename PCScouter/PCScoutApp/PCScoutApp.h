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

#include "ServerProtocolHandler.h"
#include "DocumentView.h"
#include "ScoutingDataModel.h"
#include "TabletIdentity.h"
#ifdef _XERO_BLUETOOTH_CLIENT
#include "BluetoothTransport.h"
#include "BluetoothClient.h"
#endif
#include "FormInstance.h"
#include "ImageManager.h"
#include "FormView.h"
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
#ifdef _XERO_BLE_CLIENT
	void syncWithCentralBLE();
#endif
#ifdef _XERO_BLUETOOTH_CLIENT
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
	int startScouting(const QString &key, xero::scouting::views::FormView::FormType type, const QString &title, 
		xero::scouting::datamodel::Alliance c, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

	void extractDataFromForm(int formindex, bool team, const QString &label);

	void createTeamScoutingForms();
	void createMatchScoutingForms();
	void saveCurrentForm();
	void saveForm(int formindex);
	void saveAllForms();
	void saveAndBackup();

	void mergeDataFile();

	void removeAllScoutingFormsFromViews();

	QString generateTeamTitle(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> t);
	QString generateMatchTitle(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> t);

	void startWaitForServer();

	void complete(bool reset);
	void displayLogMessage(const QString& msg);
	void errorMessage(const QString& error);
	void chooseTabletName(const QString &evkey, const QStringList& list, const QStringList &registered, QString& name);
	void processTimer();

	void startSync(xero::scouting::transport::ScoutTransport* trans);

	void debugToggled();

	void addTeam();
	void addMatch();

#ifdef _XERO_BLUETOOTH_CLIENT
	void serverConnected(xero::scouting::transport::BluetoothTransport* trans);
#endif

	void serverConnectionError(const QString& err);

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
	QMenu* add_menu_;

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

#ifdef _XERO_BLUETOOTH_CLIENT
	xero::scouting::transport::BluetoothClient* bt_client_;
#endif

#ifdef _XERO_BLE_CLIENT
	xero::scouting::transport::BLEClient* ble_client_;
#endif

	std::map<QString, std::shared_ptr<xero::scouting::datamodel::FormInstance>> form_instances_;
	xero::scouting::datamodel::ImageManager images_;

	bool host_addr_valid_;
	QHostAddress host_addr_;

	std::map<int, std::pair<bool, QString>> index_to_formtype_;
};
