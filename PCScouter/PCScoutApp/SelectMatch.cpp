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
		QBrush br(QColor(0xFF, 0xCC, 0xCC));
		item->setBackground(br);
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
		QBrush br(QColor(0xCC, 0xCC, 0xFF));
		item->setBackground(br);
		item->setData(Qt::UserRole, toString(c) + ":" + QString::number(slot));
		ui.scout_list_->addItem(item);
	}
}