#include "PCFormViewer.h"
#include "ScoutingForm.h"
#include "FormView.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QCoreApplication>
#include <memory>

using namespace xero::scouting::views;
using namespace xero::scouting::datamodel;

PCFormViewer::PCFormViewer(QWidget *parent) : QMainWindow(parent)
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

	auto form = std::make_shared<ScoutingForm>(filename);
	if (!form->isOK())
	{
		for (const QString& err : form->errors())
			logwin_->append(err);

		logwin_->append("Scout form load failed");
	}
	else
	{
		FormView* ds = dynamic_cast<FormView*>(view_frame_->getWidget(DocumentView::ViewType::TeamScoutingFormView));
		ds->setScoutingForm(form, "red");

		ds = dynamic_cast<FormView*>(view_frame_->getWidget(DocumentView::ViewType::MatchScoutingFormView));
		ds->setScoutingForm(form, "blue");
	}
}

void PCFormViewer::createWindows()
{
	QListWidgetItem* item;

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

	item = new QListWidgetItem(loadIcon("teamscout"), "Red Scouting Form", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::TeamScoutingFormView)));
	view_selector_->addItem(item);

	item = new QListWidgetItem(loadIcon("teamscout"), "Blue Scouting Form", view_selector_);
	item->setData(Qt::UserRole, QVariant(static_cast<int>(DocumentView::ViewType::MatchScoutingFormView)));
	view_selector_->addItem(item);

	top_bottom_splitter_ = new QSplitter();
	top_bottom_splitter_->setOrientation(Qt::Orientation::Vertical);
	left_right_splitter_->addWidget(top_bottom_splitter_);

	QString name = "[None]";
	view_frame_ = new DocumentView(-1, name, top_bottom_splitter_);
	view_frame_->setDataModel(nullptr);
	top_bottom_splitter_->addWidget(view_frame_);

	logwin_ = new QTextEdit(top_bottom_splitter_);
	top_bottom_splitter_->addWidget(logwin_);
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
