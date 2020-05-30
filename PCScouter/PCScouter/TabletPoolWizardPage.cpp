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