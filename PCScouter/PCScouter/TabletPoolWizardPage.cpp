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

#include "TabletPoolWizardPage.h"
#include "TabletPoolListWidget.h"
#include "AdvListWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

TabletPoolWizardPage::TabletPoolWizardPage(NewEventBlueAllianceWizard::PropertyMap& props) : props_(props)
{
	QWidget* w;

	setTitle("Tablet Scouting Pool");

	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);

	w = createTabletPoolWidget(this);
	layout->addWidget(w);

	w = createPitScoutList(this);
	layout->addWidget(w);

	w = createMatchScoutList(this);
	layout->addWidget(w);

}

TabletPoolWizardPage::~TabletPoolWizardPage()
{
}

bool TabletPoolWizardPage::isComplete() const
{
	return match_list_->count() >= 6 && pit_list_->count() > 0;
}

void TabletPoolWizardPage::setTabletList(const QStringList& tablets)
{
	for (const QString& name : tablets) {
		QListWidgetItem* item = new QListWidgetItem(name, pool_list_);
		pool_list_->addItem(item);
	}
}

QWidget* TabletPoolWizardPage::createTabletPoolWidget(QWidget* parent)
{
	QWidget* w = new QWidget(parent);
	QVBoxLayout* l = new QVBoxLayout();
	w->setLayout(l);

	QLabel* label = new QLabel("Tablet Pool", w);
	l->addWidget(label);

	pool_list_ = new TabletPoolListWidget(w);
	l->addWidget(pool_list_);

	label = new QLabel("INS key to add, DEL key to delete");
	QFont f = label->font();
	f.setPointSizeF(6.0);
	label->setFont(f);
	l->addWidget(label);

	connect(pool_list_, &QListWidget::itemChanged, this, &TabletPoolWizardPage::poolChanged);
	connect(pool_list_, &TabletPoolListWidget::tabletDeleted, this, &TabletPoolWizardPage::tabletDeleted);
	connect(pool_list_, &TabletPoolListWidget::tabletRenamed, this, &TabletPoolWizardPage::tabletRenamed);

	return w;
}

void TabletPoolWizardPage::tabletDeleted(const QString& name)
{
	for (int i = 0; i < match_list_->count(); i++) {
		if (match_list_->item(i)->text() == name)
		{
			auto item = match_list_->takeItem(i);
			delete item;
			break;
		}
	}

	for (int i = 0; i < pit_list_->count(); i++) {
		if (pit_list_->item(i)->text() == name)
		{
			auto item = pit_list_->takeItem(i);
			delete item;
			break;
		}
	}
}

void TabletPoolWizardPage::tabletRenamed(const QString& oldname, const QString &newname)
{
	for (int i = 0; i < match_list_->count(); i++) {
		if (match_list_->item(i)->text() == oldname)
		{
			match_list_->item(i)->setText(newname);
			break;
		}
	}

	for (int i = 0; i < pit_list_->count(); i++) {
		if (pit_list_->item(i)->text() == oldname)
		{
			pit_list_->item(i)->setText(newname);
			break;
		}
	}
}


void TabletPoolWizardPage::poolChanged(QListWidgetItem* item)
{
	QStringList list;

	for (int i = 0; i < pool_list_->count(); i++)
		list.push_back(pool_list_->item(i)->text());

	props_.insert_or_assign("tablets", QVariant(list));
}

QWidget* TabletPoolWizardPage::createMatchScoutList(QWidget *parent)
{
	QWidget* w = new QWidget(parent);
	QVBoxLayout* l = new QVBoxLayout();
	w->setLayout(l);

	QLabel* label = new QLabel("Match Scout List", w);
	l->addWidget(label);

	match_list_ = new AdvListWidget(w);
	l->addWidget(match_list_);
	match_list_->setDragDropMode(QAbstractItemView::DragDropMode::DropOnly);

	label = new QLabel("drag from tablet pool (minimum 6)");
	QFont f = label->font();
	f.setPointSizeF(6.0);
	label->setFont(f);
	l->addWidget(label);

	connect(match_list_, &QListWidget::itemChanged, this, &TabletPoolWizardPage::matchChanged);

	return w;
}

void TabletPoolWizardPage::matchChanged(QListWidgetItem* item)
{
	QStringList list;

	for (int i = 0; i < match_list_->count(); i++)
		list.push_back(match_list_->item(i)->text());

	props_.insert_or_assign("matches", QVariant(list));
	emit completeChanged();
}

QWidget* TabletPoolWizardPage::createPitScoutList(QWidget* parent)
{
	QWidget* w = new QWidget(parent);
	QVBoxLayout* l = new QVBoxLayout();
	w->setLayout(l);

	QLabel* label = new QLabel("Team Scout List", w);
	l->addWidget(label);

	pit_list_ = new AdvListWidget(w);
	l->addWidget(pit_list_);
	pit_list_->setDragDropMode(QAbstractItemView::DragDropMode::DropOnly);

	label = new QLabel("drag from tablet pool (minimum 1)");
	QFont f = label->font();
	f.setPointSizeF(6.0);
	label->setFont(f);
	l->addWidget(label);

	connect(pit_list_, &QListWidget::itemChanged, this, &TabletPoolWizardPage::pitChanged);

	return w;
}

void TabletPoolWizardPage::pitChanged(QListWidgetItem* item)
{
	QStringList list;

	for (int i = 0; i < pit_list_->count(); i++)
		list.push_back(pit_list_->item(i)->text());

	props_.insert_or_assign("pits", QVariant(list));
	emit completeChanged();
}