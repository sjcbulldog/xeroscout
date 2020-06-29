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

#include "PCScouter.h"
#include "FormView.h"
#include "TeamScheduleViewWidget.h"
#include "MatchViewWidget.h"
#include "DataSetViewWidget.h"
#include "TcpTransport.h"
#include "QueryViewWidget.h"
#include "AboutDialog.h"
#include "TabletIdentity.h"
#include "ChangeHistoryView.h"
#include "DataMergeListWidget.h"
#include "ZebraViewWidget.h"
#include "ScoutDataMergeDialog.h"
#include "GraphView.h"
#include "DataModelBuilder.h"
#include "PreMatchGraphView.h"
#include "AllianceGraphView.h"
#include "TeamSummaryWidget.h"
#include "PickListView.h"
#include "ZebraPatternView.h"
#include "ZebraRegionEditor.h"
#include "IntroView.h"
#include "DataGenerator.h"

#include "USBServer.h"
#include "TcpServer.h"
#include "USBTransport.h"
#include "BluetoothServer.h"

#include "NewEventAppController.h"
#include "ImportMatchDataController.h"
#include "ImportZebraDataController.h"
#include "AllTeamSummaryController.h"
#include "ImportMatchScheduleController.h"
#include "PickListController.h"
#include "ZebraAnalysisView.h"

#include "OPRCalculator.h"
#include "DPRCalculator.h"
#include "TestDataInjector.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QWizard>
#include <QTimer>
#include <QDate>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QToolBar>
#include <QImage>
#include <QPixmap>
#include <QDate>
#include <QLabel>
#include <QInputDialog>
#include <QNetworkInterface>

using namespace xero::ba;
using namespace xero::scouting::datamodel;
using namespace xero::scouting::views;
using namespace xero::scouting::transport;

/////////////////////////////////////////////////////////////
// Initialization ...
////////////////////////////////////////////////////////////

PCScouter::PCScouter(bool coach, QWidget *parent) : QMainWindow(parent), images_(true)
{
	coach_ = coach;
	sync_mgr_ = nullptr;
	coach_sync_ = nullptr;

	TestDataInjector& injector = TestDataInjector::getInstance();
	summary_progress_ = new QProgressBar();
	app_controller_ = nullptr;
	app_disabled_ = false;

	QDate now = QDate::currentDate();
	year_ = now.year();

	if (injector.hasData("year") && injector.data("year").type() == QVariant::Int)
		year_ = injector.data("year").toInt();

	createWindows();
	createMenus();

	if (coach_)
		this->setWindowTitle("PC Coach View");
	else
		this->setWindowTitle("PC Scout Central");

	if (settings_.contains("teamnumber"))
		team_number_ = settings_.value("teamnumber").toInt();
	else
		team_number_ = -1;

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());

	if (settings_.contains(LeftRightSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(LeftRightSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		left_right_splitter_->setSizes(sizes);
	}

	if (settings_.contains(TopBottomSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(TopBottomSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		top_bottom_splitter_->setSizes(sizes);
	}

	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/teamsummary.png";
	QPixmap image(imagepath);
	QIcon icon(image);
	setWindowIcon(icon);

	shutdown_client_connection_ = false;
}

void PCScouter::showEvent(QShowEvent* ev)
{
	sync_mgr_ = new SyncManager(images_, team_number_);
	connect(sync_mgr_, &SyncManager::logMessage, this, &PCScouter::logMessage);
	connect(sync_mgr_, &SyncManager::syncComplete, this, &PCScouter::saveAndBackup);
	connect(sync_mgr_, &SyncManager::enableApp, this, &PCScouter::enableApp);
	connect(sync_mgr_, &SyncManager::disableApp, this, &PCScouter::disableApp);

	blue_alliance_ = std::make_shared<BlueAlliance>();

	timer_ = new QTimer(this);
	timer_->setInterval(std::chrono::milliseconds(100));
	(void)connect(timer_, &QTimer::timeout, this, &PCScouter::processTimer);
	timer_->start();

	ping_timer_ = new QTimer(this);
	ping_timer_->setInterval(std::chrono::seconds(5));
	(void)connect(ping_timer_, &QTimer::timeout, this, &PCScouter::processPingTimer);
	ping_timer_->start();

	blue_alliance_timer_ = new QTimer(this);
	blue_alliance_timer_->setInterval(std::chrono::seconds(15));
	(void)connect(blue_alliance_timer_, &QTimer::timeout, this, &PCScouter::processBlueAllianceTimer);
	blue_alliance_timer_->start();

	socket_ = new QUdpSocket(this);
	tablet_client_ = nullptr;

	if (team_number_ == -1) {
		logwin_->append("The team number has not been set.");
		logwin_->append("  Network sync will not work.");
		logwin_->append("  Use the menu item Settings/Set Team Number to resolve.");
	}

	DataMergeListWidget* ds = dynamic_cast<DataMergeListWidget*>(view_frame_->getWidget(DocumentView::ViewType::MergeListView));
	(void)connect(ds, &DataMergeListWidget::mergePit, this, &PCScouter::mergePitRequest);
	(void)connect(ds, &DataMergeListWidget::mergeMatch, this, &PCScouter::mergeMatchRequest);

	showIPAddresses();

	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	path += "/views" + QString::number(year_) + ".json";

	if (!coach_)
		sync_mgr_->createTransports();
	else
	{
		logwin_->append("Started in coach mode");
	}

	if (datafile_.length() > 0)
	{
		auto dm = std::make_shared<ScoutingDataModel>();
		if (!dm->load(datafile_)) {
			QMessageBox::critical(this, "Error", "Could not load event data file");
			return;
		}
		setDataModel(dm);
		setupViews();

		datafile_.clear();
	}
}

void PCScouter::setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm)
{
	data_model_ = dm;
	view_frame_->setDataModel(dm);
	sync_mgr_->setDataModel(dm);

	if (data_model_ != nullptr)
	{
		connect(data_model_.get(), &ScoutingDataModel::modelChanged, this, &PCScouter::dataModelChanged);
		data_model_->blockSignals(false);

		DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchDataSet));
		ds->setDataGenerator(std::bind(&ScoutingDataModel::createMatchDataSet, data_model_, std::placeholders::_1));

		ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamDataSet));
		ds->setDataGenerator(std::bind(&ScoutingDataModel::createTeamDataSet, data_model_, std::placeholders::_1));

		ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::Predictions));
		ds->setDataGenerator(std::bind(&PCScouter::outputExpData, this, std::placeholders::_1));
	}
	else
	{
		DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchDataSet));
		ds->setDataGenerator(nullptr);

		ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamDataSet));
		ds->setDataGenerator(nullptr);
	}
}

void PCScouter::closeEvent(QCloseEvent* ev)
{
	if (data_model_ != nullptr && data_model_->isDirty()) {
		int ans = QMessageBox::question(this, "Unsaved Changes", "There are unsaved changes, do you want to save them?");
		if (ans == QMessageBox::Yes)
		{
			if (data_model_->filename().length() == 0)
			{
				saveEventAs();
			}
			else
			{
				if (!data_model_->save())
				{
					QMessageBox::critical(this, "Error", "Could not save changes, quit aborted");
					return;
				}
			}
		}
	}

	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : left_right_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(LeftRightSplitterSettings, stored);

	stored.clear();
	for (int size : top_bottom_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(TopBottomSplitterSettings, stored);
}

void PCScouter::logMessage(const QString& msg)
{
	logwin_->append(msg);
}

void PCScouter::errorMessage(const QString& msg)
{
	QMessageBox::critical(this, "Error", msg);
}

void PCScouter::createWindows()
{
	QListWidgetItem* item;

	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	view_selector_ = new SpecialListWidget(left_right_splitter_);
	(void)connect(view_selector_, &QListWidget::currentItemChanged, this, &PCScouter::listItemChanged);
	(void)connect(view_selector_, &SpecialListWidget::magicWord, this, &PCScouter::magicWordTyped);
	left_right_splitter_->addWidget(view_selector_);

	item = new QListWidgetItem(loadIcon("teamform.png"), "Introduction", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::NoModelView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Team Scouting ------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("teamform.png"), "Team Scouting Form", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamScoutingFormView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("teamstatus.png"), "Team Scouting Status", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("teamdata.png"), "Team Scouting Data", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamDataSet)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Match Scouting ------------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchform.png"), "Match Scouting Form - Red", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchScoutingFormViewRed)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchform.png"), "Match Scouting Form - Blue", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchScoutingFormViewBlue)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchstatus.png"), "Match Scouting Status", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchdata.png"), "Match Scouting Data", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchDataSet)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Analysis ------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchgraphs.png"), "Pre-Match Graphs", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchGraphView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("teamsummary.png"), "Single Team Summary", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamReport)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("allteamsummary.png"), "All Team Summary", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::AllTeamReport)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("alliancegraphs.png"), "Alliance Graphs", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::AllianceGraphView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("picklist.png"), "Pick List", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::PickListView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Zebra ------------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("zebradata.png"), "Zebra Introduction", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraIntro)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("zebradata.png"), "Zebra Region Editor", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraRegionEditor)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("pencil.png"), "Zebra Pattern Editor", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraPatternEditor)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("zebradata.png"), "Zebra Track View", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraTrackView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("zebradata.png"), "Zebra Heatmap View", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraHeatmapView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("zebradata.png"), "Zebra Replay View", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraReplayView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("sequence.png"), "Zebra Analysis", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::ZebraAnalysis)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Misc ------------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("history.png"), "Change History", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::HistoryView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("merge.png"), "Merge List", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MergeListView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("customdata.png"), "Custom Data", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::CustomDataSet)));
	view_selector_->addItem(item);

	item = new QListWidgetItem("---------- Experimental ------------------------------------------");
	item->setFlags(Qt::ItemFlag::NoItemFlags);
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("history.png"), "Predictions", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::Predictions)));
	view_selector_->addItem(item);

	view_selector_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	top_bottom_splitter_ = new QSplitter();
	top_bottom_splitter_->setOrientation(Qt::Orientation::Vertical);
	left_right_splitter_->addWidget(top_bottom_splitter_);

	view_frame_ = new DocumentView(images_, year_, "", top_bottom_splitter_);
	connect(view_frame_, &DocumentView::logMessage, this, &PCScouter::logMessage);
	connect(view_frame_, &DocumentView::switchView, this, &PCScouter::switchView);

	DataSetViewWidget * ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchDataSet));
	connect(ds, &DataSetViewWidget::rowChanged, this, &PCScouter::matchRowChanged);

	ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamDataSet));
	connect(ds, &DataSetViewWidget::rowChanged, this, &PCScouter::teamRowChanged);

	top_bottom_splitter_->addWidget(view_frame_);

	logwin_ = new QTextEdit(top_bottom_splitter_);
	top_bottom_splitter_->addWidget(logwin_);

	data_model_status_ = new QLabel("");
	statusBar()->addPermanentWidget(data_model_status_);

	ip_addr_label_ = new QLabel("[]");
	statusBar()->addPermanentWidget(ip_addr_label_);

	setDataModelStatus();
}

void PCScouter::createMenus()
{
	file_menu_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_menu_);
	(void)connect(file_menu_, &QMenu::aboutToShow, this, &PCScouter::showingFileMenu);

	if (!coach_)
	{
		file_new_event_ = file_menu_->addAction(tr("New Event"));
		(void)connect(file_new_event_, &QAction::triggered, this, &PCScouter::newEventBA);

		file_menu_->addSeparator();
	}

	file_open_event_ = file_menu_->addAction(tr("Open Event"));
	(void)connect(file_open_event_, &QAction::triggered, this, &PCScouter::openEvent);

	file_menu_->addSeparator();

	file_save_ = file_menu_->addAction(tr("Save Event"));
	(void)connect(file_save_, &QAction::triggered, this, &PCScouter::saveEvent);

	file_save_as_ = file_menu_->addAction(tr("Save Event As"));
	(void)connect(file_save_as_, &QAction::triggered, this, &PCScouter::saveEventAs);

	file_menu_->addSeparator();

	file_load_picklist_ = file_menu_->addAction(tr("Load Picklist JSON"));
	(void)connect(file_load_picklist_, &QAction::triggered, this, &PCScouter::loadPicklist);

	file_menu_->addSeparator();

	file_close_ = file_menu_->addAction(tr("Close Event"));
	(void)connect(file_close_, &QAction::triggered, this, &PCScouter::closeEventHandler);

	import_menu_ = new QMenu(tr("&Import"));
	menuBar()->addMenu(import_menu_);
	(void)connect(import_menu_, &QMenu::aboutToShow, this, &PCScouter::showingImportMenu);

	if (!coach_)
	{
		import_match_schedule_ = import_menu_->addAction(tr("Match Schedule"));
		(void)connect(import_match_schedule_, &QAction::triggered, this, &PCScouter::importMatchSchedule);
	}

	import_match_data_ = import_menu_->addAction(tr("BlueAlliance Match Results"));
	(void)connect(import_match_data_, &QAction::triggered, this, &PCScouter::importMatchData);

	import_zebra_data_ = import_menu_->addAction(tr("BlueAlliance Zebra Data"));
	(void)connect(import_zebra_data_, &QAction::triggered, this, &PCScouter::importZebraData);

	if (coach_)
	{
		sync_with_central_ = import_menu_->addAction(tr("Sync With Central"));
		(void)connect(sync_with_central_, &QAction::triggered, this, &PCScouter::syncWithCentral);
	}

	import_kpi_ = import_menu_->addAction(tr("Historical Team Performance"));
	(void)connect(import_kpi_, &QAction::triggered, this, &PCScouter::importKPIData);

	export_menu_ = new QMenu(tr("&Export"));

	menuBar()->addMenu(export_menu_);
	(void)connect(export_menu_, &QMenu::aboutToShow, this, &PCScouter::showingExportMenu);

	export_csv_ = export_menu_->addAction(tr("Current Dataset"));
	(void)connect(export_csv_, &QAction::triggered, this, &PCScouter::exportDataSet);

	settings_menu_ = new QMenu(tr("&Settings"));
	menuBar()->addMenu(settings_menu_);

	set_team_number_ = settings_menu_->addAction(tr("Set Team Number"));
	(void)connect(set_team_number_, &QAction::triggered, this, &PCScouter::setTeamNumber);

	debug_act_ = settings_menu_->addAction(tr("Debug"));
	debug_act_->setCheckable(true);
	if (settings_.contains("debug") && settings_.value("debug").toBool())
		debug_act_->setChecked(true);
	(void)connect(debug_act_, &QAction::triggered, this, &PCScouter::setDebug);

	help_menu_ = new QMenu(tr("Help"));
	menuBar()->addMenu(help_menu_);

	about_ = help_menu_->addAction(tr("About"));
	(void)connect(about_, &QAction::triggered, this, &PCScouter::about);
}

/////////////////////////////////////////////////////////////
// Synchronization ....
/////////////////////////////////////////////////////////////

void PCScouter::coachSyncError(const QString& err)
{
	QMessageBox::critical(this, "Sync Error", err);
	logwin_->append("Sync Error: " + err);
	enableApp();
	shutdown_coach_sync_ = true;
}

void PCScouter::coachSyncComplete()
{
	enableApp();
	shutdown_coach_sync_ = true;

	setDataModel(coach_sync_->dataModel());
	setupViews();

	if (data_model_->filename().length() > 0)
	{
		saveAndBackup();
	}
	else
	{
		QMessageBox::information(this, "Save File", "The event has been created, please save the data to a file");
		saveEventAs();
	}

	setDataModelStatus();
	view_frame_->needsRefreshAll();
}

void PCScouter::syncWithCentral()
{
	if (coach_sync_ != nullptr || sync_mgr_->isBusy())
		return;

	USBTransport* trans = new USBTransport();
	if (!trans->init())
	{
		QMessageBox::critical(this, "Error", "Could not initialize transport");
		return;
	}

	disableApp();
	shutdown_coach_sync_ = false;
	coach_sync_ = new CoachSync(trans, images_, debug_act_->isChecked());
	connect(coach_sync_, &CoachSync::displayLogMessage, this, &PCScouter::logMessage);
	connect(coach_sync_, &CoachSync::syncComplete, this, &PCScouter::coachSyncComplete);
	connect(coach_sync_, &CoachSync::syncError, this, &PCScouter::coachSyncError);

	if (data_model_ != nullptr)
		coach_sync_->setDataModel(data_model_);

	coach_sync_->start();
}


void PCScouter::clientTabletAttached(const TabletIdentity &id)
{
	QString protocol = tablet_client_->transportType();
	protocol = protocol[0].toUpper() + protocol.mid(1);
	QString msg = "client '" + id.name() + "' " + id.uid().toString() + " attached";
	logwin_->append(msg);
}

void PCScouter::displayMessage(const QString& msg)
{
	logwin_->append(msg);
}

void PCScouter::processPingTimer()
{
	QByteArray block;

	if (team_number_ != -1) {
		block.push_back((team_number_ >> 0) & 0xff);
		block.push_back((team_number_ >> 8) & 0xff);
		block.push_back((team_number_ >> 16) & 0xff);
		block.push_back((team_number_ >> 24) & 0xff);

		socket_->writeDatagram(block, QHostAddress::Broadcast, ClientServerProtocol::ScoutBroadcastPort);
	}
}

void PCScouter::processBlueAllianceTimer()
{
	if (blue_alliance_->state() == BlueAlliance::EngineState::Down)
		blue_alliance_->bringUp();
}

/////////////////////////////////////////////////////////////
// Main timer loop 
////////////////////////////////////////////////////////////

void PCScouter::setAppController(ApplicationController* ctrl)
{
	app_controller_ = ctrl;
	connect(ctrl, &ApplicationController::logMessage, this, &PCScouter::logMessage);
	connect(ctrl, &ApplicationController::errorMessage, this, &PCScouter::errorMessage);
}

void PCScouter::processAppController()
{
	if (!app_controller_->isDisplayInitialized())
	{
		if (app_controller_->providesProgress())
		{
			summary_progress_->setMinimum(0);
			summary_progress_->setMaximum(100);
			summary_progress_->setValue(app_controller_->percentDone());
			statusBar()->addPermanentWidget(summary_progress_);
		}

		app_controller_->setDisplayInitialized();
	}

	app_controller_->run();

	if (app_controller_->shouldDisableApp() && !app_disabled_)
		disableApp();
	else if (!app_controller_->shouldDisableApp() && app_disabled_)
		enableApp();

	if (app_controller_->isDone())
	{
		enableApp();

		if (app_controller_->providesProgress())
			statusBar()->removeWidget(summary_progress_);

		delete app_controller_;
		app_controller_ = nullptr;
	}
	else if (app_controller_->providesProgress())
	{
		summary_progress_->setValue(app_controller_->percentDone());
	}
}

void PCScouter::processTimer()
{
	if (blue_alliance_ != nullptr) {
		blue_alliance_->run();
		statusBar()->showMessage(blue_alliance_->getStatus());
	}

	sync_mgr_->run();
	if (coach_sync_ != nullptr)
	{
		if (shutdown_coach_sync_)
		{
			delete coach_sync_;
			coach_sync_ = nullptr;
		}
		else
		{
			coach_sync_->run();
		}
	}

	if (app_controller_ != nullptr)
		processAppController();

	if (shutdown_client_connection_)
	{
		delete tablet_client_;
		tablet_client_ = nullptr;
		shutdown_client_connection_ = false;
	}
}

/////////////////////////////////////////////////////////////
// Display related
////////////////////////////////////////////////////////////

void PCScouter::switchView(DocumentView::ViewType vt, const QString& key)
{
	for (int i = 0; i < view_selector_->count(); i++)
	{
		QListWidgetItem* item = view_selector_->item(i);
		const QVariant& v = item->data(Qt::UserRole);
		if (v.isValid() && v.type() == QVariant::Type::Int && v.toInt() == static_cast<int>(vt))
		{
			view_selector_->setCurrentRow(i);
			break;
		}
	}

	ViewBase* vb = dynamic_cast<ViewBase*>(view_frame_->currentWidget()); 
	if (vb != nullptr)
		vb->setKey(key);
}

void PCScouter::showIPAddresses()
{
	QString addrstr;
	int cnt = 0;

	auto list = QNetworkInterface::allAddresses();
	for (const auto& addr : list)
	{
		if (addr.isLoopback() || addr.isNull() || addr.isLinkLocal())
			continue;

		QHostAddress laddr(addr.toIPv4Address());

		if (addrstr.length() > 0)
			addrstr += ", ";
		addrstr += laddr.toString();

		cnt++;
	}

	if (cnt == 1)
		addrstr = "Address: " + addrstr;
	else
		addrstr = "Addresses: " + addrstr;

	ip_addr_label_->setText(addrstr);
}

void PCScouter::setMainView(DocumentView::ViewType type)
{
	view_selector_->setCurrentRow(-1);
	if (type == DocumentView::ViewType::NoModelView)
	{
		assert(data_model_ == nullptr);
		view_frame_->setViewType(type);
	}
	else
	{
		for (int i = 0; i < view_selector_->count(); i++)
		{
			QListWidgetItem* item = view_selector_->item(i);
			const QVariant& v = item->data(Qt::UserRole);
			if (v.isValid() && v.type() == QVariant::Type::Int && v.toInt() == static_cast<int>(type))
			{
				view_selector_->setCurrentRow(i);
				break;
			}
		}
	}
}

void PCScouter::setupViews()
{
	if (data_model_ == nullptr) 
	{
		view_frame_->clearAll();
		view_frame_->setViewType(DocumentView::ViewType::NoModelView);
	}
	else
	{
		view_frame_->setDataModel(data_model_);
		view_frame_->needsRefreshAll();

		if (view_frame_->viewType() == DocumentView::ViewType::NoModelView)
			setMainView(DocumentView::ViewType::MatchView);
	}

	setDataModelStatus();
}

QIcon PCScouter::loadIcon(const QString& filename)
{
	QIcon default;

	auto image = images_.get(filename);
	if (image == nullptr)
		return default;

	return QIcon(QPixmap::fromImage(*image));
}

void PCScouter::disableApp()
{
	QApplication::setOverrideCursor(Qt::BusyCursor);
	setEnabled(false);

	app_disabled_ = true;
}

void PCScouter::enableApp()
{
	QApplication::restoreOverrideCursor();
	setEnabled(true);

	app_disabled_ = false;
}

void PCScouter::setDataModelStatus()
{
	if (data_model_ == nullptr)
		data_model_status_->setText("Data Model: none");
	else
	{
		QString str = "Data Model";
		str += " " + data_model_->uuid().toString();
		str += " " + QString::number(data_model_->teams().size()) + " teams";
		str += " " + QString::number(data_model_->matches().size()) + " matches";
		data_model_status_->setText(str);
	}
}

////////////////////////////////////////////////////////////
// Menu Related
////////////////////////////////////////////////////////////

void PCScouter::setDebug()
{
	settings_.setValue(DebugSetting, debug_act_->isChecked());
}

void PCScouter::about()
{
	AboutDialog about;
	about.exec();
}

void PCScouter::setTeamNumber()
{
	bool ok;
	int number = QInputDialog::getInt(this, "Team Number", "Team Number:", team_number_, 0, 9999, 1, &ok);
	if (ok)
	{
		team_number_ = number;
		settings_.setValue("teamnumber", number);
	}
}

void PCScouter::showingFileMenu()
{
	bool state = (data_model_ != nullptr);

	if (blue_alliance_->state() == BlueAlliance::EngineState::Down)
		file_new_event_->setEnabled(false);
	else
		file_new_event_->setEnabled(true);

	file_save_->setEnabled(state);
	file_save_as_->setEnabled(state);
	file_load_picklist_->setEnabled(state);
	file_close_->setEnabled(state);
}

void PCScouter::showingImportMenu()
{
	bool state = (data_model_ != nullptr);

	if (blue_alliance_->state() == BlueAlliance::EngineState::Down)
		state = false;

	import_match_data_->setEnabled(state);
	import_zebra_data_->setEnabled(state);
	import_match_schedule_->setEnabled(state);
	import_kpi_->setEnabled(state);
}

void PCScouter::showingExportMenu()
{
	ViewBase* vb = dynamic_cast<ViewBase*>(view_frame_->currentWidget());
	export_csv_->setEnabled(vb != nullptr && vb->hasDataSet());
}

void PCScouter::importKPIData()
{
	QStringList teams;

	for (auto team : data_model_->teams())
		teams.push_back(team->key());

	auto ctrl = new KPIController(blue_alliance_, data_model_, data_model_->startDate(), teams, data_model_->evkey(), data_model_->teamScoutingForm(), data_model_->matchScoutingForm());
	setAppController(ctrl);
}

void PCScouter::importMatchDataComplete(bool err)
{
	saveAndBackup();
	view_frame_->needsRefreshAll();
	updateCurrentView();
}

void PCScouter::importMatchData()
{
	const char* maxmatchprop = "bamaxmatch";

	if (data_model_ == nullptr) {
		QMessageBox::critical(this, "Error", "You can only import match data into an event.  The currently no open event.  Either open an event with File/Open or create an event with File/New");
		return;
	}

	int maxmatch = std::numeric_limits<int>::max();
	TestDataInjector& injector = TestDataInjector::getInstance();

	if (injector.hasData(maxmatchprop) && injector.data(maxmatchprop).type() == QVariant::Int)
		maxmatch = injector.data(maxmatchprop).toInt();

	auto ctrl = new ImportMatchDataController(blue_alliance_, data_model_, maxmatch);
	setAppController(ctrl);
	connect(ctrl, &ApplicationController::complete, this, &PCScouter::importMatchDataComplete);
}

void PCScouter::importMatchScheduleComplete(bool err)
{
	saveAndBackup();
	view_frame_->needsRefreshAll();
	updateCurrentView();
}

void PCScouter::importMatchSchedule()
{
	const char* maxmatchprop = "bamaxmatch";

	if (data_model_ == nullptr) {
		QMessageBox::critical(this, "Error", "You can only import match data into an event.  The currently no open event.  Either open an event with File/Open or create an event with File/New");
		return;
	}

	auto ctrl = new ImportMatchScheduleController(blue_alliance_, data_model_);
	setAppController(ctrl);
	connect(ctrl, &ApplicationController::complete, this, &PCScouter::importMatchScheduleComplete);
}

void PCScouter::importZebraDataComplete(bool err)
{
	saveAndBackup();

	view_frame_->needsRefreshAll();
}

void PCScouter::importZebraData()
{
	if (data_model_ == nullptr) {
		QMessageBox::critical(this, "Error", "You can only import zebra data into an event.  The currently no open event.  Either open an event with File/Open or create an event with File/New");
		return;
	}

	auto ctrl = new ImportZebraDataController(blue_alliance_, data_model_);
	setAppController(ctrl);
	connect(ctrl, &ApplicationController::complete, this, &PCScouter::importZebraDataComplete);
}

void PCScouter::exportDataSet()
{
	DataSetViewWidget* vw = dynamic_cast<DataSetViewWidget*>(view_frame_->currentWidget());
	if (vw != nullptr)
	{
		QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
		QString filename = QFileDialog::getSaveFileName(this, "Export data file name", path, "Event Data Files (*.csv);;All Files (*.*)");
		if (!vw->dataset().writeCSV(filename))
		{
			QMessageBox::critical(this, "Error", "Cannot open file '" + filename + "' for writing");
		}
	}
	else
	{
		QMessageBox::warning(this, "Export Error", "You can only export from a view that provides a dataset");
	}
}

void PCScouter::newEventComplete(bool err)
{
	NewEventAppController* ctrl = dynamic_cast<NewEventAppController*>(app_controller_);
	assert(ctrl != nullptr);

	if (err)
	{
		view_frame_->setDataModel(nullptr);
	}
	else
	{
		settings_.setValue(TabletPoolSetting, ctrl->tablets());
		setDataModel(ctrl->dataModel());
		setupViews();

		QMessageBox::information(this, "Save File", "The event has been created, please save the data to a file");
		saveEventAs();

		setDataModelStatus();
	}
}

void PCScouter::newEventBA()
{
	if (data_model_ != nullptr && data_model_->isDirty()) {
		QMessageBox::warning(this, "Warning", "Cannot create new event, current event has unsaved changes");
		return;
	}

	QStringList tablets;
	if (settings_.contains("tablets"))
		tablets = settings_.value("tablets").toStringList();

	auto ctlr = new NewEventAppController(images_, blue_alliance_, tablets, year_);
	app_controller_ = ctlr;

	TestDataInjector& injector = TestDataInjector::getInstance();
	if (injector.hasData("nomatches") && injector.data("nomatches").toBool())
		ctlr->simNoMatches();

	(void)connect(app_controller_, &NewEventAppController::complete, this, &PCScouter::newEventComplete);
	(void)connect(app_controller_, &ApplicationController::logMessage, this, &PCScouter::logMessage);
	(void)connect(app_controller_, &ApplicationController::errorMessage, this, &PCScouter::errorMessage);
}

void PCScouter::loadPicklist()
{
	QString path;
	if (settings_.contains("FormsDir"))
		path = settings_.value("FormsDir").toString();
	else
		path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);

	QString filename = QFileDialog::getOpenFileName(this, "Open Picklist File", path, "JSON Files (*.json);;All Files (*.*)");
	if (filename.length() == 0)
		return;

	std::shared_ptr<PickListTranslator> pl = std::make_shared<PickListTranslator>();
	if (!pl->load(filename))
	{
		QMessageBox::critical(this, "Error", pl->error());
		logwin_->append(pl->error());
	}
	else
	{
		data_model_->setPickListTranslator(pl);
	}
}

void PCScouter::openEvent()
{
	if (data_model_ != nullptr && data_model_->isDirty())
	{
		int answer = QMessageBox::question(this, "Save?", "The current event has been modified, do you want to save?");
		if (answer == QMessageBox::Yes) {
			if (!data_model_->save()) {
				QMessageBox::critical(this, "Error", "Could not ave current event, open request has been canceled.");
				return;
			}
		}
	}

	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	QString filename = QFileDialog::getOpenFileName(this, "Open Event Data File", path, "Event Data Files (*.evd);;JSON Files (*.json);;All Files (*.*)");
	if (filename.length() == 0)
		return;

	auto dm = std::make_shared<ScoutingDataModel>();
	if (!dm->load(filename)) {
		QMessageBox::critical(this, "Error", "Could not load event data file");
		return;
	}
	setDataModel(dm);
	setupViews();
}

void PCScouter::saveEvent()
{
	if (data_model_ != nullptr) {
		if (data_model_->filename().length() == 0)
			saveEventAs();
		else
			data_model_->save();
	}
}

void PCScouter::saveEventAs()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	QString filename = QFileDialog::getSaveFileName(this, "Save Event Data File", path, "Event Data Files (*.evd);;JSON Files (*.json);;All Files (*.*)");
	data_model_->save(filename);
}

void PCScouter::closeEventHandler()
{
	if (data_model_ != nullptr)
	{
		if (data_model_->isDirty()) 
		{
			int answer = QMessageBox::question(this, "Save?", "The current event has been modified, do you want to save?");
			if (answer == QMessageBox::Yes) {
				if (!data_model_->save()) {
					QMessageBox::critical(this, "Error", "Could not save current event, close request has been canceled.");
					return;
				}
			}
		}

		setDataModel(nullptr);
		setMainView(DocumentView::ViewType::NoModelView);
		updateCurrentView();
	}
}

////////////////////////////////////////////////////////////
// Misc Events 
////////////////////////////////////////////////////////////

void PCScouter::teamSummaryCompleted(bool err)
{
	DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::AllTeamReport));
	assert(ds != nullptr);

	ds->refreshView();
	ds->clearNeedRefresh();
}

void PCScouter::pickListComplete(bool err)
{
	PickListView* ds = dynamic_cast<PickListView*>(view_frame_->getWidget(DocumentView::ViewType::PickListView));
	assert(ds != nullptr);

	PickListController* ctrl = dynamic_cast<PickListController*>(app_controller_);
	if (ctrl != nullptr)
	{
		ds->setHTML(ctrl->htmlPicklist(), ctrl->htmlRobotCapabilities());
		ds->clearNeedRefresh();
	}
}

void PCScouter::listItemChanged(QListWidgetItem* newitem, QListWidgetItem* olditem)
{
	if (newitem == nullptr)
		return;

	if (data_model_ == nullptr)
	{
		view_frame_->setViewType(DocumentView::ViewType::NoModelView);
	}
	else if (newitem != nullptr) {
		int index = newitem->data(Qt::UserRole).toInt();
		DocumentView::ViewType view = static_cast<DocumentView::ViewType>(index);
		view_frame_->setViewType(view);
		updateCurrentView();
	}
}

void PCScouter::updateCurrentView()
{
	auto view = view_frame_->viewType();
	switch (view)
	{
		case DocumentView::ViewType::AllTeamReport:
		{
			DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(view));
			assert(ds != nullptr);
			if (ds->needsRefresh())
			{
				auto ctrl = new AllTeamSummaryController(blue_alliance_, data_model_, ds->dataset());
				setAppController(ctrl);
				(void)connect(ctrl, &ApplicationController::complete, this, &PCScouter::teamSummaryCompleted);
			}
		}
		break;

		case DocumentView::ViewType::MatchGraphView:
		{
			const GraphDescriptorCollection& desc = data_model_->graphDescriptors();
			GraphView* ds = dynamic_cast<GraphView*>(view_frame_->getWidget(view));
			assert(ds != nullptr);
			if (!ds->hasDescriptor() && desc.has(PreMatchGraphView::Name))
			{
				ds->setDescriptor(desc[PreMatchGraphView::Name]);
				ds->refreshCharts();
				ds->clearNeedRefresh();
			}
			else if (ds->needsRefresh())
			{
				ds->refreshView();
				ds->clearNeedRefresh();
			}
		}
		break;

		case DocumentView::ViewType::AllianceGraphView:
		{
			const GraphDescriptorCollection& desc = data_model_->graphDescriptors();
			GraphView* ds = dynamic_cast<GraphView*>(view_frame_->getWidget(view));
			assert(ds != nullptr);
			if (!ds->hasDescriptor() && desc.has(AllianceGraphView::Name))
			{
				ds->setDescriptor(desc[AllianceGraphView::Name]);
				ds->refreshView();
				ds->refreshCharts();
				ds->clearNeedRefresh();
			}
			else if (ds->needsRefresh())
			{
				ds->refreshView();
				ds->refreshCharts();
				ds->clearNeedRefresh();
			}
		}
		break;

		case DocumentView::ViewType::PickListView:
		{
			PickListView* ds = dynamic_cast<PickListView*>(view_frame_->getWidget(view));
			assert(ds != nullptr);
			if (ds->needsRefresh())
			{
				if (data_model_->pickListTranslator() == nullptr)
				{
					QString html = "<b>No Picklist Configuration Loaded</b>";
					ds->setHTML(html, html);
					ds->clearNeedRefresh();
				}
				else
				{
					auto ctrl = new PickListController(blue_alliance_, team_number_, year_, data_model_, ds);
					setAppController(ctrl);
					(void)connect(ctrl, &ApplicationController::complete, this, &PCScouter::pickListComplete);
				}
			}
		}
		break;

		case DocumentView::ViewType::MatchView:
		{
			MatchViewWidget* ds = dynamic_cast<MatchViewWidget*>(view_frame_->getWidget(view));
			if (ds != nullptr)
			{
				if (ds->needsRefresh())
				{
					ds->setData(data_model_->matches());
					ds->refreshView();
					ds->clearNeedRefresh();
				}
			}
		}
		break;

		case DocumentView::ViewType::TeamView:
		{
			TeamScheduleViewWidget* ds = dynamic_cast<TeamScheduleViewWidget*>(view_frame_->getWidget(view));
			if (ds != nullptr)
			{
				if (ds->needsRefresh())
				{
					ds->setData(data_model_->teams());
					ds->refreshView();
					ds->clearNeedRefresh();
				}
			}
		}
		break;

		default:
		{
			ViewBase* vb = dynamic_cast<ViewBase*>(view_frame_->getWidget(view));
			if (vb != nullptr)
			{
				if (vb->needsRefresh())
				{
					vb->refreshView();
					vb->clearNeedRefresh();
				}
			}
		}
		break;
	}
}


////////////////////////////////////////////////////////////
// Data Model Related
////////////////////////////////////////////////////////////

void PCScouter::dataModelChanged(ScoutingDataModel::ChangeType type)
{
	//
	// We carve out some special cases because they are expensive and no update
	// is really needed, as the change was initiated by the view and the view has
	// already updated what the user sees
	//
	auto vtype = view_frame_->viewType();
	if (vtype == DocumentView::ViewType::MatchDataSet || vtype == DocumentView::ViewType::TeamDataSet)
	{
		//
		// These are datasets showing all the data, if the only changes was the rules where changed or the data column order was
		// changed, don't refresh the data set views
		//
		if (type == ScoutingDataModel::ChangeType::RulesChanged || 
			type == ScoutingDataModel::ChangeType::DataSetColumnOrder || 
			type == ScoutingDataModel::ChangeType::HistoryChanged)
			return;
	}
	else
	{
		view_frame_->needsRefreshAll();
		updateCurrentView();
	}
}

void PCScouter::saveAndBackup()
{
	if (data_model_ == nullptr)
		return;

	QString backname;
	QString filename = data_model_->filename();
	int i = 0;

	while (true)
	{
		int index = filename.lastIndexOf('.');
		if (index == -1)
		{
			backname = filename + "." + QString::number(i);
		}
		else {
			QString ext = filename.mid(index);
			QString basename = filename.mid(0, index);

			backname = basename + "." + QString::number(i) + ext;
		}

		QFile backfile(backname);
		if (!backfile.exists())
			break;

		i++;
	}

	//
	// Now we have found a new (backup) filename, rename the existing file to the new (backup) filename
	//
	QFile oldfile(filename);
	if (!oldfile.rename(backname))
	{
		QString msg;
		msg = "Could not rename file '" + filename + "' to backup name '" + backname + "', auto backups is disabled";
		QMessageBox::warning(this, "Backup Failed", msg);
	}

	data_model_->save();
}

void PCScouter::magicWordTyped(SpecialListWidget::Word w)
{
	if (w == SpecialListWidget::Word::XYZZY)
	{
		const char* redpropname = "redrandmaxmatch";
		const char* bluepropname = "bluerandmaxmatch";
		const char* teampropname = "randmaxteam";

		TestDataInjector& injector = TestDataInjector::getInstance();
		int redmaxmatch = std::numeric_limits<int>::max();
		int bluemaxmatch = std::numeric_limits<int>::max();
		int teammax = std::numeric_limits<int>::max();

		if (injector.hasData(redpropname) && injector.data(redpropname).type() == QVariant::Int)
			redmaxmatch = injector.data(redpropname).toInt();

		if (injector.hasData(bluepropname) && injector.data(bluepropname).type() == QVariant::Int)
			bluemaxmatch = injector.data(bluepropname).toInt();

		if (injector.hasData(teampropname) && injector.data(teampropname).type() == QVariant::Int)
			teammax = injector.data(teampropname).toInt();

		DataGenerator gen(data_model_, year_, redmaxmatch, bluemaxmatch, teammax);
		if (gen.isValid())
		{
			connect(&gen, &DataGenerator::logMessage, this, &PCScouter::logMessage);
			gen.run();

			QMessageBox::information(this, "DataSet", "The dataset has been populated");
			saveAndBackup();
		}
		else
		{
			QMessageBox::warning(this, "No Generator", "There is no data generator for the year '" + QString::number(year_) + "'");
		}
	}
}

void PCScouter::findPointFields(QStringList& list)
{
	list.clear();

	auto m = data_model_->matches().front();
	if (!m->hasExtraField("ba_totalPoints"))
		return;

	for (auto f : *m->extraData(Alliance::Red, 1))
	{
		if (f.first.endsWith("Points") && f.first != "ba_totalPoints")
			list.push_back(f.first);
	}
}

void PCScouter::outputExpData(ScoutingDataSet& ds)
{
	int nopredict = 5;
	Alliance c;
	QStringList fields = { "ba_autoCellPoints", "ba_autoInitLinePoints", "ba_controlPanelPoints", "ba_endgamePoints", "ba_teleopCellPoints", "ba_foulPoints" };
	std::vector<OPRCalculator*> oprs;

	findPointFields(fields);
	ds.clear();

	if (fields.size() == 0)
		return;

	OPRCalculator opr(data_model_, "ba_totalPoints");
	opr.calc();

	DPRCalculator dpr(opr);
	dpr.calc();

	for (const QString& field : fields)
	{
		auto opr = new OPRCalculator(data_model_, field);
		opr->calc();
		oprs.push_back(opr);
	}
	qDebug() << "Added " << oprs.size() << " OPR Fields";

	ds.addHeader(std::make_shared<FieldDesc>("MatchKey", FieldDesc::Type::String, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Red Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Blue Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("R1", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("R2", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("R3", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("B1", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("B2", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("B3", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Red OPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Blue OPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("OPR Delta", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Red DPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Blue DPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Red NPR Scoure", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Blue NPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("NPR Delta", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Red Partial OPR Scoure", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Blue Partial OPR Score", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Partial OPR Delta", FieldDesc::Type::Double, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Actual Winner", FieldDesc::Type::String, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("OPR Winner", FieldDesc::Type::String, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("NPR Winner", FieldDesc::Type::String, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Partial OPR Winner", FieldDesc::Type::String, false, true));
	ds.addHeader(std::make_shared<FieldDesc>("Predicted", FieldDesc::Type::String, false, true));


	int row = 0;
	int miss = 1;
	int none = 1;
	for (auto m : data_model_->matches())
	{
		if (!m->hasExtraField("ba_totalPoints"))
			continue;

		ds.newRow();
		ds.addData(m->key());

		auto it = m->extraData(Alliance::Red, 1)->find("ba_totalPoints");
		assert(it != m->extraData(Alliance::Red, 1)->end());
		double redscore = it->second.toDouble();
		ds.addData(redscore);

		it = m->extraData(Alliance::Blue, 1)->find("ba_totalPoints");
		assert(it != m->extraData(Alliance::Blue, 1)->end());
		double bluescore = it->second.toDouble();
		ds.addData(bluescore);

		//
		// Compute predicted match scores based on OPR and NPR
		//
		double redopr = 0;
		double reddpr = 0;
		double blueopr = 0;
		double bluedpr = 0;
		double rednpr;
		double bluenpr;

		Alliance w[3], wact;

		c = Alliance::Red;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = m->team(c, slot);
			auto t = data_model_->findTeamByKey(team);
			ds.addData(t->number());
			redopr += opr[team];
			reddpr += dpr[team];
		}

		c = Alliance::Blue;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = m->team(c, slot);
			auto t = data_model_->findTeamByKey(team);
			ds.addData(t->number());
			blueopr += opr[team];
			bluedpr += dpr[team];
		}

		reddpr /= 3.0;
		bluedpr /= 3.0;

		ds.addData(redopr);
		ds.addData(blueopr);
		ds.addData(std::fabs(redopr - blueopr));

		ds.addData(reddpr);
		ds.addData(bluedpr);

		rednpr = redopr * bluedpr ;
		bluenpr = blueopr * reddpr;

		ds.addData(rednpr);
		ds.addData(bluenpr);
		ds.addData(std::fabs(rednpr - bluenpr));

		int redpartial = 0;
		int bluepartial = 0;

		c = Alliance::Red;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = m->team(c, slot);
			for (auto one : oprs)
			{
				qDebug() << "team " << team << " field " << one->field() << " value " << (*one)[team];
				redpartial += (*one)[team];
			}
		}

		c = Alliance::Blue;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = m->team(c, slot);
			for (auto one : oprs)
			{
				qDebug() << "team " << team << " field " << one->field() << " value " << (*one)[team];
				bluepartial += (*one)[team];
			}
		}

		ds.addData(redpartial);
		ds.addData(bluepartial);
		ds.addData(std::fabs(redpartial - bluepartial));

		
		if (redscore > bluescore)
		{
			wact = Alliance::Red;
			ds.addData("RED");
		}
		else
		{
			wact = Alliance::Blue;
			ds.addData("BLUE");
		}

		if (std::fabs(redopr - blueopr) < nopredict)
		{
			w[0] = Alliance::Invalid;
			ds.addData("");
		}
		else if (redopr > blueopr)
		{
			w[0] = Alliance::Red;
			ds.addData("RED");
		}
		else
		{
			w[0] = Alliance::Blue;
			ds.addData("BLUE");
		}

		if (std::fabs(rednpr - bluenpr) < nopredict)
		{
			w[1] = Alliance::Invalid;
			ds.addData("");
		}
		else if (rednpr > bluenpr)
		{
			w[1] = Alliance::Red;
			ds.addData("RED");
		}
		else
		{
			w[1] = Alliance::Blue;
			ds.addData("BLUE");
		}

		if (std::fabs(redpartial - bluepartial) < nopredict)
		{
			w[2] = Alliance::Invalid;
			ds.addData("");
		}
		else if (redpartial > bluepartial)
		{
			w[2] = Alliance::Red;
			ds.addData("RED");
		}
		else
		{
			w[2] = Alliance::Blue;
			ds.addData("BLUE");
		}

		int bcnt = 0;
		int rcnt = 0;
		for (int i = 0; i < 3; i++)
		{
			if (w[i] == Alliance::Red)
				rcnt++;
			else if (w[i] == Alliance::Blue)
				bcnt++;
		}

		Alliance predicted = Alliance::Invalid;
		if (rcnt > bcnt)
			predicted = Alliance::Red;
		else if (bcnt > rcnt)
			predicted = Alliance::Blue;

		if (predicted != Alliance::Invalid && predicted != wact)
			ds.addData("MISS-" + QString::number(miss++));
		else if (predicted == Alliance::Invalid)
			ds.addData("NONE-" + QString::number(none++));
		else
			ds.addData("OK");

		row++;
	}
}

void PCScouter::printProfileError(const QString& err)
{
	logwin_->append("GameProfile Descriptor File: " + err);
}

void PCScouter::mergePitRequest(const QString& key)
{
	auto t = data_model_->findTeamByKey(key);
	ScoutDataMergeDialog dialog(data_model_->teamScoutingForm(), t->teamScoutingDataList());
	if (dialog.exec() == QDialog::Accepted)
	{
		data_model_->setTeamScoutingData(key, dialog.result(), true);
		DataMergeListWidget* ds = dynamic_cast<DataMergeListWidget*>(view_frame_->getWidget(DocumentView::ViewType::MergeListView));
		ds->needsRefresh();
		setMainView(DocumentView::ViewType::MergeListView);
	}
}

void PCScouter::mergeMatchRequest(const QString& key, xero::scouting::datamodel::Alliance c, int slot)
{
	auto m = data_model_->findMatchByKey(key);
	ScoutDataMergeDialog dialog(data_model_->matchScoutingForm(), m->scoutingDataList(c, slot));
	if (dialog.exec() == QDialog::Accepted)
	{
		data_model_->setMatchScoutingData(key, c, slot, dialog.result(), true);
		DataMergeListWidget* ds = dynamic_cast<DataMergeListWidget*>(view_frame_->getWidget(DocumentView::ViewType::MergeListView));
		ds->needsRefresh();
		setMainView(DocumentView::ViewType::MergeListView);
	}
}

void PCScouter::teamRowChanged(int row, int col)
{
	DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamDataSet));
	auto& set = ds->dataset();

	int keycol = set.getColumnByName("TeamKey");
	QString tkey = set.get(row, keycol).toString();
	QString colname = set.colHeader(col)->name();

	data_model_->changeTeamData(tkey, colname, set.get(row, col));
}

void PCScouter::matchRowChanged(int row, int col)
{
	DataSetViewWidget* ds = dynamic_cast<DataSetViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchDataSet));
	assert(ds != nullptr);

	auto& set = ds->dataset();

	int keycol = set.getColumnByName("MatchKey");
	QString mkey = set.get(row, keycol).toString();

	keycol = set.getColumnByName("MatchTeamKey");
	QString tkey = set.get(row, keycol).toString();

	QString colname = set.colHeader(col)->name();

	data_model_->changeMatchData(mkey, tkey, colname, set.get(row, col));
}