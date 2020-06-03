//
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

#include "PCScoutApp.h"
#include "FormView.h"
#include "TeamScheduleViewWidget.h"
#include "MatchViewWidget.h"
#include "TcpTransport.h"
#include "USBTransport.h"
#include "AboutDialog.h"
#include "ClientServerProtocol.h"
#include "BluetoothClient.h"
#include "BluetoothTransport.h"
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

using namespace xero::scouting::datamodel;
using namespace xero::scouting::views;
using namespace xero::scouting::transport;

PCScoutApp::PCScoutApp(QWidget *parent) : QMainWindow(parent), images_(false)
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

	data_model_ = std::make_shared<ScoutingDataModel>(ScoutingDataModel::Role::ScoutingTablet);

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

	bt_client_ = nullptr;
	host_addr_valid_ = false;
}

void PCScoutApp::mergeDataFile()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	path += data_model_->evkey() + identity_.name() + ".ced";

	QFile file(path);
	if (file.exists())
	{
		auto m = data_model_->findMatchByKey("2020orore_qm1");
		data_model_->loadScoutingDataJSON(path);
		m = data_model_->findMatchByKey("2020orore_qm1");
		data_model_->removeOldScoutingData();
		m = data_model_->findMatchByKey("2020orore_qm1");
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
	FormView* view;

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

	auto viewt = view_frame_->viewType();
	setMainView(viewt);
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

	item = new QListWidgetItem(loadIcon("teams"), "Team Scouting", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("schedule"), "Match Scouting", view_selector_);
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

	act = sync_menu_->addAction(tr("Bluetooth Sync"));
	(void)connect(act, &QAction::triggered, this, &PCScoutApp::syncWithCentralBluetooth);

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

void PCScoutApp::extractDataFromForm(int view, const QString &label)
{
	if (view_frame_->isForm(view)) {

		QStringList pieces = label.split(':');

		if (pieces.size() == 2 && pieces.front() == "Team") {
			// Pit form
			int tnum = pieces[1].toInt();
			auto t = data_model_->findTeamByNumber(tnum);
			FormView* fv = dynamic_cast<FormView*>(view_frame_->getWidget(view));

			ScoutingDataMapPtr p = std::make_shared<ScoutingDataMap>();
			fv->extractData(p);
			p->insert_or_assign("TabletName", identity_.name());
			p->insert_or_assign("TabletUUID", identity_.uid().toString());
			p->insert_or_assign("TimeStamp", QDateTime::currentDateTime().toString());
			data_model_->setTeamScoutingData(t->key(), p, true);
		}
		else if (pieces.size() == 4 && pieces.front() == "Match") {
			// Match form
			QString type = pieces[1];
			int set = pieces[2].toInt();
			int match = pieces[3].toInt();

			auto m = data_model_->findMatchByTriple(type, set, match);
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
		QString label = olditem->text();
		extractDataFromForm(view, label);
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
		saveAndBackup();

	ignore_view_select_changes_ = true;
	removeAllScoutingFormsFromViews();
	ignore_view_select_changes_ = false;

	identity_ = TabletIdentity(identity_.uid());
	settings_.setValue(TabletNameKey, "");

	data_model_ = std::make_shared<ScoutingDataModel>(ScoutingDataModel::Role::ScoutingTablet);

	view_frame_->setTablet("[NONE]");
	tablet_name_->setText("[NO NAME]");

	setupViews();

	QString msg = "Data Model: " + QString::number(data_model_->teams().size()) + " teams, " +
		QString::number(data_model_->teams().size()) + " matches";
	data_model_display_->setText(msg);
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
}

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

void PCScoutApp::serverConnectionError(const QString& err)
{
	delete bt_client_;
	bt_client_ = nullptr;
	QMessageBox::critical(this, "Error", "Could not connect to selected central machine - " + err);
}

void PCScoutApp::serverConnected(BluetoothTransport *trans)
{
	delete bt_client_;
	bt_client_ = nullptr;
	startSync(trans);
}

void PCScoutApp::syncWithCentralUSB()
{
	saveAllForms();
	if (data_model_ != nullptr && data_model_->filename().length() > 0)
		saveAndBackup();

	setEnabled(false);
	logwin_->append("Starting USB scouting data synchronization");
	auto* trans = new USBTransport();
	if (!trans->init())
	{
		setEnabled(true);
		QMessageBox::critical(this, "USB Error", "Cannot initialize USB connection to central");
		return;
	}
	logwin_->append("USB Connected: " + trans->description());
	startSync(trans);
}

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
	tcp_error_ = connect(tcp_socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &PCScoutApp::connectError);

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

		index = startScouting(key, "team", title, Alliance::Red, data_model_->teamScoutingForm());
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
		index = startScouting(key, "match", title , c, data_model_->matchScoutingForm());
		saveForm(index);

		MatchViewWidget* w = dynamic_cast<MatchViewWidget*>(view_frame_->getWidget(DocumentView::ViewType::MatchView));
		w->setScoutingField(key, c, slot, true);
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

int PCScoutApp::startScouting(const QString& key, const QString &type, const QString &title, Alliance c, std::shared_ptr<const ScoutingForm> form)
{
	QListWidgetItem* item;
	int index;
	QColor color;

	if (type == "team")
		color = QColor(0, 128, 0);
	else if (c == Alliance::Red)
		color = QColor(255, 0, 0);
	else
		color = QColor(0, 0, 255);

	//
	// Create the view
	//
	if (view_frame_->createFetchFormView(key, title, color, index))
	{
		QString title;

		//
		// Put the view into the view selector
		//
		if (type == "team") {
			auto team = data_model_->findTeamByKey(key);
			title = "Team:" + QString::number(team->number());
		}
		else if (type == "match") {
			auto match = data_model_->findMatchByKey(key);
			title = "Match:" + match->compType() + ":" + QString::number(match->set()) + ":" + QString::number(match->match());
		}
		else {
			assert(false);
		}

		if (!viewExists(index))
		{
			item = new QListWidgetItem(loadIcon("form"), title, view_selector_);
			item->setData(Qt::UserRole, QVariant(index));
			view_selector_->addItem(item);
		}
	}

	//
	// Bind the view to a scouting form
	//
	FormView* view = dynamic_cast<FormView*>(view_frame_->getWidget(index));
	assert(view != nullptr);
	view->setScoutingForm(form, toString(c));

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
			Alliance c;
			int slot;

			QString title = generateTeamTitle(t);
			int index = startScouting(t->key(), "team", title, Alliance::Red, data_model_->teamScoutingForm());

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
				int index = startScouting(m->key(), "match", title, c, data_model_->matchScoutingForm());

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

	extractDataFromForm(viewindex, label);
}

void PCScoutApp::saveAllForms()
{
	for (int i = static_cast<int>(DocumentView::ViewType::FirstFormView); i < view_frame_->count(); i++)
		saveForm(i);

	setupViews();
}

QString PCScoutApp::generateTeamTitle(std::shared_ptr<const DataModelTeam> t)
{
	return "Team Scouting: " + QString::number(t->number()) + " - " + t->name();
}

QString PCScoutApp::generateMatchTitle(std::shared_ptr<const DataModelMatch> m, std::shared_ptr<const DataModelTeam> t)
{
	QString qmtxt;

	qmtxt = m->compType() + " ";
	if (m->compType() != "qm")
	{
		qmtxt += "set " + QString::number(m->set());
		qmtxt += ", match " + QString::number(m->match());
	}
	return qmtxt + ", Team " + QString::number(t->number());
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
	int index = startScouting(team->key(), "team", title, Alliance::Red, data_model_->teamScoutingForm());
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
	int index = startScouting(mkey, "match", title, c, data_model_->matchScoutingForm());
	saveForm(index);
}
