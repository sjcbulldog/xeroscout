#include "SelectMatch.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>

using namespace xero::scouting::datamodel;

SelectMatch::SelectMatch(std::shared_ptr<ScoutingDataModel> dm, QWidget *parent) : QDialog(parent)
{
	dm_ = dm;
	ui.setupUi(this);

	QPushButton* button;

	button = ui.box_->button(QDialogButtonBox::StandardButton::Ok);
	connect(button, &QPushButton::pressed, this, &QDialog::accept);

	button = ui.box_->button(QDialogButtonBox::StandardButton::Cancel);
	connect(button, &QPushButton::pressed, this, &QDialog::reject);

	connect(ui.match_list_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &SelectMatch::indexChanged);
	connect(ui.scout_list_, &QListWidget::itemDoubleClicked, this, &SelectMatch::itemDouble);

	for (auto m : dm_->matches())
	{
		ui.match_list_->addItem(m->title(), m->key());
	}
	ui.match_list_->setCurrentIndex(0);
}

SelectMatch::~SelectMatch()
{
}

void SelectMatch::itemDouble(QListWidgetItem* item)
{
	accept();
}

void SelectMatch::indexChanged(int index)
{
	match_key_ = ui.match_list_->currentData().toString();
	auto m = dm_->findMatchByKey(match_key_);

	while (ui.scout_list_->count() > 0)
	{
		auto item = ui.scout_list_->takeItem(0);
		delete item;
	}

	if (m == nullptr)
		return;

	Alliance c = Alliance::Red;
	for (int slot = 1; slot <= 3; slot++)
	{
		QString tm = m->team(c, slot);
		auto t = dm_->findTeamByKey(tm);
		QListWidgetItem* item = new QListWidgetItem(QString::number(t->number()) + " - " + t->name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setBackgroundColor(QColor(0xFF, 0xCC, 0xCC));
		item->setData(Qt::UserRole, toString(c) + ":" + QString::number(slot));
		ui.scout_list_->addItem(item);
	}

	c = Alliance::Blue;
	for (int slot = 1; slot <= 3; slot++)
	{
		QString tm = m->team(c, slot);
		auto t = dm_->findTeamByKey(tm);
		QListWidgetItem* item = new QListWidgetItem(QString::number(t->number()) + " - " + t->name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setBackgroundColor(QColor(0xCC, 0xCC, 0xFF));
		item->setData(Qt::UserRole, toString(c) + ":" + QString::number(slot));
		ui.scout_list_->addItem(item);
	}
}