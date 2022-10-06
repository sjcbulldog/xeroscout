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

#include "CoachSync.h"
#include "ScoutingDataModel.h"
#include "DocumentView.h"
#include "BlueAlliance.h"
#include "ScoutServer.h"
#include "ClientProtocolHandler.h"
#include "SpecialListWidget.h"
#include "ApplicationController.h"
#include "KPIController.h"
#include "SyncManager.h"
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
	PCScouter(bool coach, QWidget *parent = Q_NULLPTR);

	void setDataFile(const QString& file) {
		datafile_ = file;
	}

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
	void readPreferences();

	void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	void setAppController(ApplicationController* ctrl);

	void enableApp();
	void disableApp();

	void createWindows();
	void createMenus();

	void dataModelChanged(xero::scouting::datamodel::ScoutingDataModel::ChangeType type);

	void processTimer();
	void processPingTimer();
	void processBlueAllianceTimer();
	QIcon loadIcon(const QString& filename);
	void listItemChanged(QListWidgetItem* olditem, QListWidgetItem* newitem);

	// File Menu
	void showingFileMenu();
	void newEventBA();
	void newEventOffSeason();

	void openEvent();
	void saveEvent();
	void saveEventAs();
	void closeEventHandler();

	// Run Menu
	void showingRunMenu();
	void recentImage(const QString& name);

	// Import Menu
	void showingImportMenu();
	void importMatchSchedule();
	void loadOffseasonMatchSchedule();
	void importMatchScheduleComplete(bool err);
	void importMatchData();
	void importMatchDataComplete(bool err);
	void importZebraData();
	void importZebraDataComplete(bool err);
	void importKPIData();

	// Export menu
	void showingExportMenu();
	void exportDataSet();

	// Settings menu
	void setTeamNumber();

	// Help menu
	void about();

	void newBAEventComplete(bool err);
	void newOffseasonEventComplete(bool err);

	void saveAndBackup();
	void setDataModelStatus();

	void updateCurrentView();
	void syncWithCentral();

	void displayMessage(const QString &msg);

	void clientTabletAttached(const xero::scouting::datamodel::TabletIdentity& id);
	void setupViews();
	void setMainView(xero::scouting::views::DocumentView::ViewType type);

	void magicWordTyped(SpecialListWidget::Word w);

	void mergePitRequest(const QString& key);
	void mergeMatchRequest(const QString& key, xero::scouting::datamodel::Alliance c, int slot);

	void showIPAddresses();
	void setDebug();

	void printProfileError(const QString& err);

	void teamSummaryCompleted(bool err);
	void pickListComplete(bool err);

	void processAppController();
	void runPicklistProgram();
	void resetPicklist();
	void createImageView();
	void createImageView2(const QString& filename, bool addrecent);

	void switchView(xero::scouting::views::DocumentView::ViewType vt, const QString& key);
	void logMessage(const QString& msg);
	void errorMessage(const QString& msg);
	void coachSyncError(const QString& msg);
	void coachSyncComplete();

	void teamRowChanged(int row, int col);
	void matchRowChanged(int row, int col);

	void outputExpData(xero::scouting::datamodel::ScoutingDataSet& ds);
	void findPointFields(QStringList& list);

private:
	// 
	// Timer for background activities that must be done
	// on the main thread
	//
	QTimer* timer_;
	QTimer* ping_timer_;
	QTimer* blue_alliance_timer_;
	QProgressBar* summary_progress_;
	CoachSync *coach_sync_;
	bool shutdown_coach_sync_;

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
	QAction* file_new_event_;
	QAction* file_new_offseason_event_;
	QAction* file_open_event_;
	QAction* file_save_;
	QAction* file_save_as_;
	QAction* file_close_;

	QMenu* run_menu_;
	QAction* run_picklist_program_;
	QAction* reset_picklist_;
	QAction* create_image_view_;
	QMenu* recent_image_view_;

	QMenu* import_menu_;
	QAction *import_calc_opr_;
	QAction* import_match_data_;
	QAction* import_zebra_data_;
	QAction* import_match_schedule_;
	QAction* import_offseason_schedule_;
	QAction* import_kpi_;
	QAction* sync_with_central_;

	QMenu* export_menu_;
	QAction* export_csv_;

	QMenu* settings_menu_;
	QAction* debug_act_;
	QAction* set_team_number_;

	QMenu* help_menu_;
	QAction* about_;

	//
	// Application settings
	//
	QSettings settings_;

	std::shared_ptr<xero::ba::BlueAlliance> blue_alliance_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> data_model_;

	QStringList team_keys_;

	//
	// The name of the picklist program
	//
	QString picklist_program_;

	QUdpSocket* socket_;
	QTcpServer* server_;
	ClientProtocolHandler* tablet_client_;
	bool shutdown_client_connection_ ;

	QString query_;
	int year_;

	QLabel* data_model_status_;
	QLabel* ip_addr_label_;

	int team_number_;

	ApplicationController* app_controller_;
	bool app_disabled_;

	xero::scouting::datamodel::ImageManager images_;

	SyncManager* sync_mgr_;

	bool coach_;

	QString datafile_;

	QStringList recent_image_views_;
	QStringList image_views_created_;
};
