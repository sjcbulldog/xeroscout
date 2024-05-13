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

#include "PCFormViewer.h"
#include "ScoutingForm.h"
#include "FormView.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QCoreApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <memory>

using namespace xero::scouting::views;
using namespace xero::scouting::datamodel;

PCFormViewer::PCFormViewer(QWidget *parent) : QMainWindow(parent), images_(true)
{
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

	form_ = std::make_shared<ScoutingForm>();
}

void PCFormViewer::closeEvent(QCloseEvent* ev)
{
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

QIcon PCFormViewer::loadIcon(const QString& filename)
{
	QString iconfile = QCoreApplication::applicationDirPath() + "/" + filename;
	QPixmap image(iconfile);
	return QIcon(image);
}

void PCFormViewer::listItemChanged(QListWidgetItem* newitem, QListWidgetItem* olditem)
{
	if (newitem != nullptr)
	{
		int index = newitem->data(Qt::UserRole).toInt();
		DocumentView::ViewType view = static_cast<DocumentView::ViewType>(index);
		view_frame_->setViewType(view);
	}
}

void PCFormViewer::loadForm()
{
	QString dir;

	if (settings_.contains("DIRPATH"))
		dir = settings_.value("DIRPATH").toString();

	QString filename = QFileDialog::getOpenFileName(this, "Open Form JSON file", dir, "Event Data Files (*.json);");

	QFileInfo info(filename);
	dir = info.absoluteDir().absolutePath();
	settings_.setValue("DIRPATH", dir);

	form_ = std::make_shared<ScoutingForm>(filename);
	if (!form_->isOK())
	{
		for (const QString& err : form_->errors())
			logwin_->append(err);

		logwin_->append("Scout form load failed");
		form_ = nullptr;
	}
	else
	{
		updateWindow();
	}
}

void PCFormViewer::updateWindow()
{
	QListWidgetItem* item;

	model_ = std::make_shared<ScoutingDataModel>("key", "name", QDate::currentDate(), QDate::currentDate());
	model_->setScoutingForm(form_);
	view_frame_->setDataModel(model_);

	view_selector_->clear();

	if (form_->formType() == "team") {
		item = new QListWidgetItem(loadIcon("matchform"), "Team Form", view_selector_);
		item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamScoutingFormView)));
		view_selector_->addItem(item);
	}
	else {
		item = new QListWidgetItem(loadIcon("matchform"), "Red Match Form", view_selector_);
		item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchScoutingFormViewRed)));
		view_selector_->addItem(item);

		item = new QListWidgetItem(loadIcon("matchform"), "Blue Match Form", view_selector_);
		item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchScoutingFormViewBlue)));
		view_selector_->addItem(item);
	}
	view_frame_->refreshAll();
}

void PCFormViewer::createWindows()
{
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	view_selector_ = new QListWidget(left_right_splitter_);
	(void)connect(view_selector_, &QListWidget::currentItemChanged, this, &PCFormViewer::listItemChanged);
	left_right_splitter_->addWidget(view_selector_);
	QFont f = view_selector_->font();
	f.setPointSizeF(18.0);
	view_selector_->setFont(f);

	view_selector_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);



	top_bottom_splitter_ = new QSplitter();
	top_bottom_splitter_->setOrientation(Qt::Orientation::Vertical);
	left_right_splitter_->addWidget(top_bottom_splitter_);

	QString name = "[None]";
	view_frame_ = new DocumentView(images_, QDate::currentDate().year(), name, top_bottom_splitter_);
	view_frame_->setDataModel(nullptr);
	top_bottom_splitter_->addWidget(view_frame_);

	logwin_ = new QTextEdit(top_bottom_splitter_);
	top_bottom_splitter_->addWidget(logwin_);
}

void PCFormViewer::updatePerForm(std::shared_ptr<xero::scouting::datamodel::ScoutingForm> form)
{

}

void PCFormViewer::createMenus()
{
	QAction* act;

	file_menu_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_menu_);

	act = file_menu_->addAction(tr("Load Form"));
	(void)connect(act, &QAction::triggered, this, &PCFormViewer::loadForm);

	act = file_menu_->addAction(tr("Exit"));
	(void)connect(act, &QAction::triggered, this, &PCFormViewer::close);
}
