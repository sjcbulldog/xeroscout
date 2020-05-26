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

#include "ScoutingDataModel.h"
#include "DocumentView.h"
#include "BlueAlliance.h"
#include "ScoutServer.h"
#include "ClientProtocolHandler.h"
#include "SpecialListWidget.h"
#include "AllTeamSummaryGenerator.h"
#include "GameRandomProfile.h"
#include "ApplicationController.h"
#include "KPIController.h"
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QFrame>
#include <QString>
#include <QTextEdit>
#include <QSettings>
#include <QIcon>
#include <QUdpSocket>
#include <QTcpServer>
#include <QLabel>
#include <QProgressBar>

class PCScouter : public QMainWindow
{
	Q_OBJECT

public:
	PCScouter(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent* ev);
	void showEvent(QShowEvent* ev);

private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windows";
	static constexpr const char* LeftRightSplitterSettings = "leftright";
	static constexpr const char* TopBottomSplitterSettings = "topbottom";
	static constexpr const char* EventNameEntry = "eventname";
	static constexpr const char* TabletPoolSetting = "tablets";
	static constexpr const char* DebugSetting = "debug";

private:
	void enableApp();
	void disableApp();

	void createWindows();
	void createMenus();
	void createTransports();

	void dataModelChanged(xero::scouting::datamodel::ScoutingDataModel::ChangeType type);

	void processTimer();
	void processPingTimer();
	QIcon loadIcon(const QString& filename);
	void listItemChanged(QListWidgetItem* olditem, QListWidgetItem* newitem);

	// Menu actions
	void newEventBA();
	void newEventComplete(bool err);

	void openEvent();
	void saveEvent();
	void saveEventAs();
	void closeEventHandler();

	void importMatchData();
	void importMatchDataComplete(bool err);

	void importZebraData();
	void importZebraDataComplete(bool err);

	void importKPIData();
	void exportScoutingData();
	void showingImportMenu();
	void setTeamNumber();
	void about();

	void saveAndBackup();
	void setDataModelStatus();

	void clientError(const QString &errmsg);
	void displayMessage(const QString &msg);
	void clientComplete();
	void clientDisconnected();

	void clientTabletAttached(const xero::scouting::datamodel::TabletIdentity& id);
	void setupViews();
	void setMainView(xero::scouting::views::DocumentView::ViewType type);

	void syncWithTablet(xero::scouting::transport::ScoutTransport* trans);

	void magicWordTyped(SpecialListWidget::Word w);

	void mergePitRequest(const QString& key);
	void mergeMatchRequest(const QString& key, xero::scouting::datamodel::DataModelMatch::Alliance c, int slot);

	void showIPAddresses();
	void setDebug();

	void printProfileError(const QString& err);

	void processArguments();
	void teamSummaryCompleted(bool err);

	void processAppController();

	void logMessage(const QString& msg);
	void errorMessage(const QString& msg);

private:
	// 
	// Timer for background activities that must be done
	// on the main thread
	//
	QTimer* timer_;
	QTimer* ping_timer_;
	QProgressBar* summary_progress_;

	//
	// Window related items
	//
	QSplitter* left_right_splitter_;
	QSplitter* top_bottom_splitter_;
	SpecialListWidget* view_selector_;
	xero::scouting::views::DocumentView* view_frame_;
	QWidget* database_view_;
	QTextEdit* logwin_;

	//
	// Menu related items
	//
	QMenu *file_menu_;
	QMenu* import_menu_;
	QAction* import_match_data_;
	QAction* import_zebra_data_;
	QMenu* export_menu_;
	QMenu* settings_menu_;
	QAction* debug_act_;
	QMenu* help_menu_;

	//
	// Application settings
	//
	QSettings settings_;

	std::shared_ptr<xero::ba::BlueAlliance> blue_alliance_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data_model_;

	QStringList team_keys_;

	QUdpSocket* socket_;
	QTcpServer* server_;
	ClientProtocolHandler* tablet_client_;
	bool shutdown_client_connection_ ;

	QString query_;
	int year_;

	std::list<std::shared_ptr<xero::scouting::transport::ScoutServer>> transport_servers_;

	QLabel* data_model_status_;
	QLabel* ip_addr_label_;

	int team_number_;

	xero::scouting::datamodel::GameRandomProfile random_profile_;

	ApplicationController* app_controller_;
	bool app_disabled_;
};
