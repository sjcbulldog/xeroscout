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

#include "PCScoutApp.h"
#include "FormView.h"
#include "TeamScheduleViewWidget.h"
#include "MatchViewWidget.h"
#include "TcpTransport.h"
#include "USBTransport.h"
#include "AboutDialog.h"
#include "ClientServerProtocol.h"

#ifdef _XERO_BLE_CLIENT
#include "BLETransport.h"
#endif

#ifdef _XERO_BLUETOOTH_CLIENT
#include "BluetoothClient.h"
#include "BluetoothTransport.h"
#endif

#include "SelectMatch.h"
#include <QMenuBar>
#include <QMenu>
#include <QCoreApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QStatusBar>
#include <QStandardPaths>

#include <sstream>

using namespace xero::scouting::datamodel;
using namespace xero::scouting::views;
using namespace xero::scouting::transport;

PCScoutApp::PCScoutApp(QWidget* parent) : QMainWindow(parent), images_(false)
{
	server_ = nullptr;
	ignore_view_select_changes_ = false;

	if (settings_.contains(TabletGUIDKey))
	{
		identity_ = TabletIdentity(QUuid(settings_.value(TabletGUIDKey).toByteArray()));
	}
	else
	{
		//
		// If there was not a UUID stored in the settings, we don't have one yet, so the one
		// created as part of this object is the new UID for the tablet.  Store it in the
		// settings right away.
		//
		settings_.setValue(TabletGUIDKey, identity_.uid().toByteArray());
	}

	//
	// This is kind of a hack.  There is a problem with some of the tablets where older bugs
	// allowed a tablet to generate a zero UUID and store it.  Once it is stored, it is somewhat
	// permanent, but we don't want zero UUIDs.  This cleans up the UUID stored in permanent
	// storage if it happens to be zero
	//
	bool zero = true;
	QByteArray ba = identity_.uid().toByteArray();
	for (int i = 0; i < ba.size(); i++) {
		if (ba.at(i) != 0)
		{
			zero = false;
			break;
		}
	}

	if (zero) {
		QUuid uid;
		identity_ = TabletIdentity(uid);
	}

	if (settings_.contains(TabletNameKey))
	{
		identity_ = TabletIdentity(settings_.value(TabletNameKey).toString(), identity_.uid());
	}

	data_model_ = std::make_shared<ScoutingDataModel>();

	createWindows();
	createMenus();

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

	if (identity_.name().length() == 0)
		tablet_name_->setText("[NO NAME]");
	else
		tablet_name_->setText(identity_.name());

	uuid_display_->setText(identity_.uid().toString());


	if (settings_.contains("teamnumber"))
		team_number_ = settings_.value("teamnumber").toInt();
	else
		team_number_ = -1;

	if (settings_.contains(CurrentEventKey) && identity_.name().length() > 0)
	{
		QString evkey = settings_.value(CurrentEventKey).toString();
		QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
		path += evkey + identity_.name() + ".ced";

		QFile file(path);
		if (file.exists())
		{
			data_model_->load(path);
			QString msg = "Data Model: " + QString::number(data_model_->teams().size()) + " teams, " +
				QString::number(data_model_->teams().size()) + " matches";
			data_model_display_->setText(msg);
		}
	}

	setupViews();

	createTeamScoutingForms();
	createMatchScoutingForms();

	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/form.png";
	QPixmap image(imagepath);
	QIcon icon(image);
	setWindowIcon(icon);

	if (settings_.contains(DebugParamName) && settings_.value(DebugParamName).toBool())
		debug_act_->setChecked(true);

#ifdef _XERO_BLUETOOTH_CLIENT
	bt_client_ = nullptr;
#endif

#ifdef _XERO_BLE_CLIENT
	ble_client_ = nullptr;
#endif

	host_addr_valid_ = false;
}

void PCScoutApp::mergeDataFile()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	path += data_model_->evkey() + identity_.name() + ".ced";

	QFile file(path);
	if (file.exists())
	{
		data_model_->loadScoutingDataJSON(path);
		data_model_->removeOldScoutingData();
	}
}

void PCScoutApp::saveAndBackup()
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
	if (oldfile.exists()) {
		if (!oldfile.rename(backname))
		{
			QString msg;
			msg = "Could not rename file '" + filename + "' to backup name '" + backname + "', auto backups is disabled";
			QMessageBox::warning(this, "Backup Failed", msg);
		}
	}

	data_model_->save();
}

void PCScoutApp::closeEvent(QCloseEvent* ev)
{
	saveAllForms();

	if (data_model_->filename().length() > 0)
		saveAndBackup();

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

void PCScoutApp::showEvent(QShowEvent* ev)
{
	state_ = State::Ready;
	server_ = nullptr;

	timer_ = new QTimer(this);
	timer_->setInterval(std::chrono::milliseconds(1));
	(void)connect(timer_, &QTimer::timeout, this, &PCScoutApp::processTimer);
	timer_->start();

	udp_socket_ = new QUdpSocket(this);
	udp_socket_->bind(ClientServerProtocol::ScoutBroadcastPort, QAbstractSocket::ShareAddress);
	connect(udp_socket_, &QUdpSocket::readyRead, this, &PCScoutApp::readBroadcast);
}

void PCScoutApp::processTimer()
{
	switch (state_)
	{
	case State::Shutdown:
		if (server_ != nullptr)
		{
			delete server_;
			server_ = nullptr;
		}
		setEnabled(true);
		setupViews();
		state_ = State::Ready;
		break;

	case State::Synchronizing:
		if (server_ != nullptr)
			server_->run();
		break;
	}
}

void PCScoutApp::setupViews()
{
	TeamScheduleViewWidget* tv = dynamic_cast<TeamScheduleViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamView));
	std::list<std::shared_ptr<const DataModelTeam>> teams;
	for (auto t : data_model_->teams()) {
		if (t->tablet() == identity_.name())
			teams.push_back(t);
	}
	tv->setData(teams);

	MatchViewWidget* mv = dynamic_cast<MatchViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchView));

	std::list<std::shared_ptr<const DataModelMatch>> matches;
	for (auto m : data_model_->matches()) {
		if (m->needsTablet(identity_.name()))
			matches.push_back(m);
	}
	mv->setData(matches);
}

void PCScoutApp::displayLogMessage(const QString& msg)
{
	logwin_->append(msg);
}

void PCScoutApp::complete(bool reset)
{
	state_ = State::Shutdown;

	logwin_->append("Synchronization completed sucesfully");

	if (reset)
	{
		mergeDataFile();

		identity_ = server_->identity();
		tablet_name_->setText(identity_.name());
		uuid_display_->setText(identity_.uid().toString());
		view_frame_->setTablet(identity_.name());
	}

	view_frame_->needsRefreshAll();

	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	path += data_model_->evkey() + identity_.name() + ".ced";
	data_model_->setFilename(path);
	saveAndBackup();

	QString msg = "Data Model: " + QString::number(data_model_->teams().size()) + " teams, " + 
		QString::number(data_model_->teams().size()) + " matches";
	data_model_display_->setText(msg);

	MatchViewWidget* mv = dynamic_cast<MatchViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchView));
	if (mv != nullptr)
		mv->setTablet(identity_.name());

	TeamScheduleViewWidget* pv = dynamic_cast<TeamScheduleViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamView));
	if (pv != nullptr)
		pv->setTablet(identity_.name());

	tablet_name_->setText(identity_.name());

	if (reset)
	{
		removeAllScoutingFormsFromViews();
		createTeamScoutingForms();
		createMatchScoutingForms();
		settings_.setValue(CurrentEventKey, data_model_->evkey());
	}

	setMainView(view_frame_->viewType());
}

void PCScoutApp::errorMessage(const QString& error)
{
	logwin_->append(error);
	QMessageBox::critical(this, "Error", error);
	state_ = State::Shutdown;
}

QIcon PCScoutApp::loadIcon(const QString& filename)
{
	QString iconfile = QCoreApplication::applicationDirPath() + "/" + filename;
	QPixmap image(iconfile);
	return QIcon(image);
}

void PCScoutApp::createWindows()
{
	QListWidgetItem* item;

	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	view_selector_ = new QListWidget(left_right_splitter_);
	(void)connect(view_selector_, &QListWidget::currentItemChanged, this, &PCScoutApp::listItemChanged);
	left_right_splitter_->addWidget(view_selector_);
	QFont f = view_selector_->font();
	f.setPointSizeF(18.0);
	view_selector_->setFont(f);

	item = new QListWidgetItem(loadIcon("teamstatus"), "Team Scouting", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("matchstatus"), "Match Scouting", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchView)));
	view_selector_->addItem(item);

	view_selector_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	top_bottom_splitter_ = new QSplitter();
	top_bottom_splitter_->setOrientation(Qt::Orientation::Vertical);
	left_right_splitter_->addWidget(top_bottom_splitter_);

	QString name = identity_.name();
	if (name.length() == 0)
		name = "[None]";
	view_frame_ = new DocumentView(images_, -1, name, top_bottom_splitter_);
	view_frame_->setDataModel(data_model_);
	top_bottom_splitter_->addWidget(view_frame_);

	logwin_ = new QTextEdit(top_bottom_splitter_);
	top_bottom_splitter_->addWidget(logwin_);

	data_model_display_ = new QLabel("Data Model");
	statusBar()->addPermanentWidget(data_model_display_);

	tablet_name_ = new QLabel("[NO NAME]");
	statusBar()->addPermanentWidget(tablet_name_);

	uuid_display_ = new QLabel("[NO ID]");
	statusBar()->addPermanentWidget(uuid_display_);

	(void)connect(view_frame_, &DocumentView::itemDoubleClicked, this, &PCScoutApp::viewItemDoubleClicked);
}

void PCScoutApp::createMenus()
{
	QAction* act;
	double size = 18.0;

	QFont f = menuBar()->font();
	f.setPointSizeF(size);
	menuBar()->setFont(f);

	sync_menu_ = new QMenu(tr("&Synchronization"));
	f = menuBar()->font();
	f.setPointSizeF(size);
	sync_menu_->setFont(f);

	menuBar()->addMenu(sync_menu_);

	act = sync_menu_->addAction(tr("Network Sync (Automatic)"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralNetwork);

	act = sync_menu_->addAction(tr("Network Sync (Direct)"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralNetworkDirect);

	act = sync_menu_->addAction(tr("USB Sync"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralUSB);

#ifdef _XERO_BLUETOOTH_CLIENT
	act = sync_menu_->addAction(tr("Bluetooth Sync"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralBluetooth);
#endif

#ifdef _XERO_BLE_CLIENT
	act = sync_menu_->addAction(tr("BLE Sync"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralBLE);
#endif

	settings_menu_ = new QMenu(tr("&Settings"));
	f = menuBar()->font();
	f.setPointSizeF(size);
	settings_menu_->setFont(f);
	menuBar()->addMenu(settings_menu_);

	act = settings_menu_->addAction(tr("Reset Tablet"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::resetTablet);

	act = settings_menu_->addAction(tr("Set Team Number"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::setTeamNumber);

	settings_menu_->addSeparator();

	debug_act_ = settings_menu_->addAction(tr("Debug"));
	debug_act_->setCheckable(true);
	(void)connect(debug_act_, &QAction::triggered, this, &PCScoutApp::debugToggled);

	add_menu_ = new QMenu(tr("&Add"));
	f = menuBar()->font();
	f.setPointSizeF(size);
	add_menu_->setFont(f);
	menuBar()->addMenu(add_menu_);

	act = add_menu_->addAction(tr("Team Scouting Assignment"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::addTeam);

	act = add_menu_->addAction(tr("Match Scouting Assignment"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::addMatch);

	help_menu_ = new QMenu(tr("Help"));
	f = menuBar()->font();
	f.setPointSizeF(size);
	help_menu_->setFont(f);
	menuBar()->addMenu(help_menu_);

	act = help_menu_->addAction(tr("About"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::about);
}

void PCScoutApp::debugToggled()
{
	settings_.setValue(DebugParamName, QVariant(debug_act_->isChecked()));
}

void PCScoutApp::about()
{
	AboutDialog about;
	about.exec();
}

void PCScoutApp::toggleTeamsViewed(bool checked)
{
	setupViews();
}

void PCScoutApp::setMainView(DocumentView::ViewType type)
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

void PCScoutApp::extractDataFromForm(int view, bool team, const QString &key)
{
	if (view_frame_->isForm(view)) {
		if (team)
		{
			auto t = data_model_->findTeamByKey(key);
			FormView* fv = dynamic_cast<FormView*>(view_frame_->getWidget(view));

			ScoutingDataMapPtr p = std::make_shared<ScoutingDataMap>();
			fv->extractData(p);
			p->insert_or_assign("TabletName", identity_.name());
			p->insert_or_assign("TabletUUID", identity_.uid().toString());
			p->insert_or_assign("TimeStamp", QDateTime::currentDateTime().toString());
			data_model_->setTeamScoutingData(t->key(), p, true);
		}
		else
		{
			auto m = data_model_->findMatchByKey(key);
			FormView* fv = dynamic_cast<FormView*>(view_frame_->getWidget(view));
			ScoutingDataMapPtr p = std::make_shared<ScoutingDataMap>();
			fv->extractData(p);
			p->insert_or_assign("TabletName", identity_.name());
			p->insert_or_assign("TabletUUID", identity_.uid().toString());
			p->insert_or_assign("TimeStamp", QDateTime::currentDateTime().toString());

			Alliance color;
			int slot;
			m->tabletToAllianceSlot(identity_.name(), color, slot);
			data_model_->setMatchScoutingData(m->key(), color, slot, p, true);
		}
	}
}

void PCScoutApp::listItemChanged(QListWidgetItem* newitem, QListWidgetItem* olditem)
{
	if (ignore_view_select_changes_)
		return;

	if (olditem != nullptr) {
		int view = olditem->data(Qt::UserRole).toInt();
		if (view >= static_cast<int>(DocumentView::ViewType::FirstFormView))
		{
			auto it = index_to_formtype_.find(view);
			assert(it != index_to_formtype_.end());
			extractDataFromForm(view, it->second.first, it->second.second);
		}
	}

	if (data_model_ == nullptr)
	{
		view_frame_->setViewType(DocumentView::ViewType::NoModelView);
	}
	else if (newitem != nullptr) {
		int index = newitem->data(Qt::UserRole).toInt();
		DocumentView::ViewType view = static_cast<DocumentView::ViewType>(index);
		view_frame_->setViewType(view);
	}
}

void PCScoutApp::resetTablet()
{
	//
	// Save the data assocaited with the old tablet name is the data file
	// based on that old name
	//
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
	{
		saveAndBackup();
	}

	ignore_view_select_changes_ = true;
	removeAllScoutingFormsFromViews();
	ignore_view_select_changes_ = false;

	identity_ = TabletIdentity(identity_.uid());
	settings_.setValue(TabletNameKey, "");

	data_model_ = std::make_shared<ScoutingDataModel>();
	view_frame_->setDataModel(data_model_);

	view_frame_->setTablet("[NONE]");
	tablet_name_->setText("[NO NAME]");

	setupViews();

	QString msg = "Data Model: " + QString::number(data_model_->teams().size()) + " teams, " +
		QString::number(data_model_->teams().size()) + " matches";
	data_model_display_->setText(msg);

	QMessageBox::information(this, "Success", "The tablet has been reset");
}

void PCScoutApp::removeAllScoutingFormsFromViews()
{
	bool changed = true;

	int i = view_selector_->count() - 1;
	while (true)
	{
		QListWidgetItem* item = view_selector_->item(i);
		int index = item->data(Qt::UserRole).toInt();
		if (!view_frame_->isForm(index))
			break;

		//
		// Remove the view item, and remove the form
		//
		view_frame_->deleteWidget(index);
		item = view_selector_->takeItem(i);
		delete item;

		i--;
	}

	view_frame_->setViewType(DocumentView::ViewType::TeamView);
}

void PCScoutApp::setTeamNumber()
{
	bool ok;
	int number = QInputDialog::getInt(this, "Team Number", "Team Number:", team_number_, 0, 9999, 1, &ok);
	if (ok)
	{
		team_number_ = number;
		settings_.setValue("teamnumber", number);
	}
}

void PCScoutApp::startWaitForServer()
{
	saveAllForms();
	setEnabled(false);
	state_ = State::WaitingForServer;

	logwin_->append("Waiting for central beacon ... ");

}

void PCScoutApp::syncWithCentralNetwork()
{
	if (team_number_ == -1)
	{
		QMessageBox::warning(this, "No Team Number", "This tablet has not been given a team number, use the Settings/Set Team Number menu item to resolve");
		return;
	}

	saveAllForms();
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
		saveAndBackup();

	if (!host_addr_valid_)
	{
		QMessageBox::warning(this, "No Central Found", "This tablet has not heard from a central tablet.  Waiting may help, but doing a direct sync is the best remedy");
		return;
	}

	QHostAddress addr2(host_addr_.toIPv4Address());
	logwin_->append("Central machine " + addr2.toString() + " found, establishing data connection");

	tcp_socket_ = new QTcpSocket(this);
	tcp_socket_->connectToHost(host_addr_, ClientServerProtocol::ScoutStreamPort);

	tcp_connect_ = connect(tcp_socket_, &QTcpSocket::connected, this, &PCScoutApp::streamConnected);
	state_ = State::WaitingForConnection;
}

void PCScoutApp::startSync(ScoutTransport* trans)
{
	server_ = new ServerProtocolHandler(identity_, images_, data_model_, trans, 1, debug_act_->isChecked());

	server_complete_connect_ = connect(server_, &ServerProtocolHandler::complete, this, &PCScoutApp::complete);
	server_display_connect_ = connect(server_, &ServerProtocolHandler::displayLogMessage, this, &PCScoutApp::displayLogMessage);
	server_error_connect_ = connect(server_, &ServerProtocolHandler::errorMessage, this, &PCScoutApp::errorMessage);
	server_choose_tablet_connect_ = connect(server_, &ServerProtocolHandler::chooseTabletName, this, &PCScoutApp::chooseTabletName);

	state_ = State::Synchronizing;
	server_->startSync();
}

#ifdef _XERO_BLUETOOTH_CLIENT
void PCScoutApp::syncWithCentralBluetooth()
{
	if (team_number_ == -1)
	{
		QMessageBox::warning(this, "No Team Number", "This tablet has not been given a team number, use the Settings/Set Team Number menu item to resolve");
		return;
	}

	saveAllForms();
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
		saveAndBackup();

	setEnabled(false);

	bt_client_ = new BluetoothClient(team_number_);
	connect(bt_client_, &BluetoothClient::connected, this, &PCScoutApp::serverConnected);
	connect(bt_client_, &BluetoothClient::connectError, this, &PCScoutApp::serverConnectionError);

	if (!bt_client_->search())
	{
		delete bt_client_;
		bt_client_ = nullptr;
		return;
	}
}
#endif

void PCScoutApp::serverConnectionError(const QString& err)
{
#ifdef _XERO_BLUETOOTH_CLIENT
	delete bt_client_;
	bt_client_ = nullptr;
#endif

#ifdef _XERO_BLE_CLIENT
	delete ble_client_;
	ble_client_ = nullptr;
#endif

	QMessageBox::critical(this, "Error", "Could not connect to selected central machine - " + err);
}

#ifdef _XERO_BLUETOOTH_CLIENT
void PCScoutApp::serverConnected(BluetoothTransport *trans)
{
	delete bt_client_;
	bt_client_ = nullptr;
	startSync(trans);
}
#endif

void PCScoutApp::syncWithCentralUSB()
{
	std::stringstream messages;

	saveAllForms();
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
		saveAndBackup();

	setEnabled(false);
	logwin_->append("Starting USB scouting data synchronization");
	auto* trans = new USBTransport();
	if (!trans->init(messages))
	{
		std::string msg;
		messages.seekg(0);
		while (std::getline(messages, msg)) {
			logwin_->append(QString::fromStdString(msg));
		}

		setEnabled(true);
		QMessageBox::critical(this, "USB Error", "Cannot initialize USB connection to central");
		return;
	}
	logwin_->append("USB Connected: " + trans->description());
	startSync(trans);
}

#ifdef _XERO_BLE_CLIENT
void PCScoutApp::syncWithCentralBLE()
{
	std::stringstream messages;

	saveAllForms();
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
		saveAndBackup();

	setEnabled(false);
	logwin_->append("Starting BLE scouting data synchronization");
	auto* trans = new BLETransport(false);
	if (!trans->init(messages))
	{
		std::string msg;
		messages.seekg(0);
		while (std::getline(messages, msg)) {
			logwin_->append(QString::fromStdString(msg));
		}

		setEnabled(true);
		QMessageBox::critical(this, "USB Error", "Cannot initialize USB connection to central");
		return;
	}
	logwin_->append("BLE Connected: " + trans->description());
	startSync(trans);
}
#endif


void PCScoutApp::debugOutput(const QString& out)
{
	if (debug_act_->isChecked())
		logwin_->append(out);
}

void PCScoutApp::syncWithCentralNetworkDirect()
{
	bool ok;

	if (settings_.contains(ServerIPAddressName))
	{
		server_ip_ = settings_.value(ServerIPAddressName).toString();
	}

	server_ip_ = QInputDialog::getText(this, "Central IP Address", "Central IP Address", QLineEdit::Normal, server_ip_, &ok);
	if (!ok)
		return;


	setEnabled(false);

	settings_.setValue(ServerIPAddressName, server_ip_);
	tcp_socket_ = new QTcpSocket(this);

	tcp_connect_ = connect(tcp_socket_, &QTcpSocket::connected, this, &PCScoutApp::streamConnected);
	tcp_error_ = connect(tcp_socket_,&QAbstractSocket::errorOccurred, this, &PCScoutApp::connectError);

	tcp_socket_->connectToHost(server_ip_, ClientServerProtocol::ScoutStreamPort);
	state_ = State::WaitingForConnection;
}

void PCScoutApp::readBroadcast()
{
	QHostAddress addr;
	quint16 port;
	QByteArray data;
	bool found = false;

	while (udp_socket_->hasPendingDatagrams()) {
		data.resize(udp_socket_->pendingDatagramSize());
		udp_socket_->readDatagram(data.data(), data.size(), &addr, &port);

		if (data.size() == 4) {
			uint8_t b0 = data[0];
			uint8_t b1 = data[1];
			uint8_t b2 = data[2];
			uint8_t b3 = data[3];

			int team = static_cast<uint32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
			if (team == team_number_)
			{
				host_addr_valid_ = true;
				host_addr_ = addr;

				QHostAddress addr(host_addr_.toIPv4Address());
				statusBar()->showMessage("Central: " + addr.toString());
			}
		}
	}
}


void PCScoutApp::connectError(QAbstractSocket::SocketError err)
{
	disconnect(tcp_connect_);
	disconnect(tcp_error_);
	delete tcp_socket_;

	QMessageBox::critical(this, "Connection Error", "Failed to connect to central machine");
	setEnabled(true);
}

void PCScoutApp::streamConnected()
{
	disconnect(tcp_connect_);
	TcpTransport* trans = new TcpTransport(tcp_socket_);
	tcp_socket_ = nullptr;
	logwin_->append("TCP connection established - synchronization starting");

	startSync(trans);
}

void PCScoutApp::viewItemDoubleClicked(DocumentView::ViewType t, const QString& key)
{
	int index;

	if (t == DocumentView::ViewType::TeamView)
	{
		auto team = data_model_->findTeamByKey(key);
		QString title = generateTeamTitle(team);

		index = startScouting(key, FormView::FormType::Team, title, Alliance::Red, data_model_->teamScoutingForm());
		saveForm(index);

		TeamScheduleViewWidget* w = dynamic_cast<TeamScheduleViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::TeamView));
		w->setScoutingField(key, true);
	}
	else if (t == DocumentView::ViewType::MatchView)
	{
		auto m = data_model_->findMatchByKey(key);
		Alliance c;
		int slot;

		m->tabletToAllianceSlot(identity_.name(), c, slot);
		auto team = data_model_->findTeamByKey(m->team(c, slot));

		QString title = generateMatchTitle(m, team);
		index = startScouting(key, FormView::FormType::Match, title , c, data_model_->matchScoutingForm());
		saveForm(index);

		MatchViewWidget* w = dynamic_cast<MatchViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchView));
		w->setScoutingField(key, c, slot);
	}

	for (int i = 0; i < view_selector_->count(); i++) {
		QListWidgetItem* item = view_selector_->item(i);
		if (item->data(Qt::UserRole).toInt() == i)
		{
			view_selector_->setCurrentItem(item);
			break;
		}
	}
}

void PCScoutApp::setScoutingView(int viewindex)
{
	for (int i = 0; i < view_selector_->count(); i++) {
		QListWidgetItem* item = view_selector_->item(i);
		if (item->data(Qt::UserRole).toInt() == viewindex)
		{
			view_selector_->setCurrentItem(item);
			break;
		}
	}
}

bool PCScoutApp::viewExists(int viewindex)
{
	bool ret = false;

	for (int i = 0; i < view_selector_->count(); i++) {
		QListWidgetItem* item = view_selector_->item(i);
		if (item->data(Qt::UserRole).toInt() == viewindex)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

int PCScoutApp::startScouting(const QString& key, FormView::FormType type, const QString &title, Alliance c, std::shared_ptr<const ScoutingForm> form)
{
	QListWidgetItem* item;
	int index;
	QColor color;
	bool teamstate;

	if (type == FormView::FormType::Team)
		color = QColor(0, 128, 0);
	else if (c == Alliance::Red)
		color = QColor(255, 0, 0);
	else
		color = QColor(0, 0, 255);

	//
	// Create the view
	//
	if (view_frame_->createFetchFormView(key, title, color, type, c, index))
	{
		QString title;
		QString icon;

		//
		// Put the view into the view selector
		//
		if (type == FormView::FormType::Team) {
			auto team = data_model_->findTeamByKey(key);
			title = "Team:" + QString::number(team->number());
			icon = "teamform";
			teamstate = true;
		}
		else if (type == FormView::FormType::Match) {
			auto match = data_model_->findMatchByKey(key);
			title = match->title();
			icon = "matchform";
			teamstate = false;
		}
		else {
			assert(false);
		}

		if (!viewExists(index))
		{
			item = new QListWidgetItem(loadIcon(icon), title, view_selector_);
			item->setData(Qt::UserRole, QVariant(index));
			view_selector_->addItem(item);
			index_to_formtype_.insert_or_assign(index, std::make_pair(teamstate, key));
		}
	}

	//
	// Bind the view to a scouting form
	//
	FormView* view = dynamic_cast<FormView*>(view_frame_->getWidget(index));
	assert(view != nullptr);
	view->refreshView();

	setScoutingView(index);
	return index;
}

void PCScoutApp::createTeamScoutingForms()
{
	std::list<std::shared_ptr<const DataModelTeam>> teams = data_model_->teams();
	teams.sort([](std::shared_ptr<const DataModelTeam> a, std::shared_ptr<const DataModelTeam> b) -> bool
		{
			return a->number() < b->number();
		});

	for (auto t : teams)
	{
		if (t->teamScoutingData() != nullptr)
		{
			QString title = generateTeamTitle(t);
			int index = startScouting(t->key(), FormView::FormType::Team, title, Alliance::Red, data_model_->teamScoutingForm());

			FormView* form = dynamic_cast<FormView*>(view_frame_->getWidget(index));
			assert(form != nullptr);

			auto sd = t->teamScoutingData();
			form->assignData(sd);
		}
	}
}

void PCScoutApp::createMatchScoutingForms()
{
	for (auto m : data_model_->matches())
	{
		Alliance c;
		int slot;

		if (m->tabletToAllianceSlot(identity_.name(), c, slot)) {
			if (m->hasScoutingData(c, slot))
			{
				auto t = data_model_->findTeamByKey(m->team(c, slot));
				QString title = generateMatchTitle(m, t);
				int index = startScouting(m->key(), FormView::FormType::Match, title, c, data_model_->matchScoutingForm());

				FormView* form = dynamic_cast<FormView*>(view_frame_->getWidget(index));
				assert(form != nullptr);

				auto sd = m->data(c, slot);
				form->assignData(sd);
			}
		}
	}
}

void PCScoutApp::saveCurrentForm()
{
	QListWidgetItem* item = view_selector_->currentItem();
	assert(item->data(Qt::UserRole).type() == QVariant::Type::Int);

	int viewindex = item->data(Qt::UserRole).toInt();
	if (viewindex >= static_cast<int>(DocumentView::ViewType::FirstFormView))
		saveForm(viewindex);
}

void PCScoutApp::saveForm(int viewindex)
{
	assert(viewindex >= static_cast<int>(DocumentView::ViewType::FirstFormView) && viewindex < view_frame_->count());

	QString label;
	bool found = false;

	for (int i = 0; i < view_selector_->count(); i++) {
		auto item = view_selector_->item(i);
		assert(item->data(Qt::UserRole).type() == QVariant::Type::Int);

		if (item->data(Qt::UserRole).toInt() == viewindex)
		{
			found = true;
			label = item->text();
			break;
		}
	}
	assert(found == true);

	auto it = index_to_formtype_.find(viewindex);
	assert(it != index_to_formtype_.end());
	extractDataFromForm(viewindex, it->second.first, it->second.second);
}

void PCScoutApp::saveAllForms()
{
	for (int i = static_cast<int>(DocumentView::ViewType::FirstFormView); i < view_frame_->count(); i++)
		saveForm(i);

	setupViews();
}

QString PCScoutApp::generateTeamTitle(std::shared_ptr<const DataModelTeam> t)
{
	return "Team Scouting: " + QString::number(t->number()) + " - " + t->nick();
}

QString PCScoutApp::generateMatchTitle(std::shared_ptr<const DataModelMatch> m, std::shared_ptr<const DataModelTeam> t)
{
	Alliance color;
	int slot;

	m->tabletToAllianceSlot(identity_.name(), color, slot);
	QString colorstr;

	colorstr = (color == Alliance::Blue) ? "Blue" : "Red";
	return m->title() + " - " + colorstr + " - " + QString::number(t->number());
}

void PCScoutApp::chooseTabletName(const QString &evkey, const QStringList& list, const QStringList &registered, QString& choice)
{
	QInputDialog d;

	QStringList items;

	for (const QString& name : list)
	{
		QString item = name;
		if (registered.contains(name))
			item += "*";
		items.push_back(item);
	}

	d.setComboBoxItems(items);
	d.setComboBoxEditable(false);
	d.setLabelText("Select Tablet Name");

	if (d.exec() == QDialog::Accepted)
	{
		choice = d.textValue();
		if (choice.endsWith('*'))
			choice = choice.mid(0, choice.length() - 1);

		identity_ = TabletIdentity(choice, identity_.uid());
		settings_.setValue(TabletGUIDKey, identity_.uid().toString());
		settings_.setValue(TabletNameKey, identity_.name());
	}
}

void PCScoutApp::addTeam()
{
	QStringList list;
	bool ok;

	for (auto t : data_model_->teams())
		list.push_back(QString::number(t->number()));

	QString item = QInputDialog::getItem(this, "Pick Team", "Team", list, 0, false, &ok);
	if (!ok)
		return;

	int num = item.toInt();

	std::shared_ptr<const DataModelTeam> team;
	for (auto t : data_model_->teams())
	{
		if (t->number() == num)
		{
			team = t;
			break;
		}
	}

	assert(team != nullptr);
	data_model_->assignTeamTablet(team->key(), identity_.name());
	view_frame_->refreshAll();

	QString title = generateTeamTitle(team);
	int index = startScouting(team->key(), FormView::FormType::Team, title, Alliance::Red, data_model_->teamScoutingForm());
	saveForm(index);
}

void PCScoutApp::addMatch()
{
	SelectMatch dialog(data_model_, this);

	if (dialog.exec() == QDialog::Rejected)
		return;

	QString mkey = dialog.matchKey();
	QString which = dialog.whichPlayer();

	Alliance c;
	int slot;

	if (which.startsWith("red"))
	{
		c = Alliance::Red;
		which = which.mid(4);
		slot = which.toInt();
	}
	else
	{
		c = Alliance::Blue;
		which = which.mid(5);
		slot = which.toInt();
	}

	data_model_->assignMatchTablet(mkey, c, slot, identity_.name());
	view_frame_->refreshAll();

	auto m = data_model_->findMatchByKey(mkey);
	QString tkey = m->team(c, slot);
	auto team = data_model_->findTeamByKey(tkey);
	QString title = generateMatchTitle(m, team);
	int index = startScouting(mkey, FormView::FormType::Match, title, c, data_model_->matchScoutingForm());
	saveForm(index);
}
