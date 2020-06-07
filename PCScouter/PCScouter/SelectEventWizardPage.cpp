//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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


#include "SelectEventWizardPage.h"
#include "BlueAllianceEngine.h"
#include <QTreeWidget>
#include <QBoxLayout>
#include <QDebug>

using namespace xero::ba;

SelectEventWizardPage::SelectEventWizardPage(NewEventBlueAllianceWizard::PropertyMap &props, BlueAllianceEngine &engine) : props_(props)
{
	QStringList headers = { "District", "Date", "Event" };

	setTitle("Select Event");

	QTreeWidget* tree = new QTreeWidget();
	tree->setColumnCount(3);
	tree->setHeaderLabels(headers);

	auto& evs = engine.events();

	for (auto& pair : evs) {
		QTreeWidgetItem* item = new QTreeWidgetItem(tree);
		if (pair.second->district() != nullptr)
			item->setText(0, pair.second->district()->name());
		item->setText(1, pair.second->start().toString() + " - " + pair.second->end().toString());
		item->setText(2, pair.second->name());

		item->setData(0, Qt::UserRole, pair.first);
		item->setData(1, Qt::UserRole, pair.second->name());

		tree->addTopLevelItem(item);
	}

	tree->resizeColumnToContents(0);
	tree->resizeColumnToContents(1);
	tree->resizeColumnToContents(2);

	tree->sortByColumn(0);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(tree);
	setLayout(layout);

	(void)connect(tree, &QTreeWidget::currentItemChanged, this, &SelectEventWizardPage::eventChanged);
	(void)connect(tree, &QTreeWidget::itemDoubleClicked, this, &SelectEventWizardPage::doubleClicked);
}

SelectEventWizardPage::~SelectEventWizardPage()
{
}

void SelectEventWizardPage::eventChanged(QTreeWidgetItem* newitem, QTreeWidgetItem* olditem)
{
	if (olditem != nullptr) {
		QString key = newitem->data(0, Qt::UserRole).toString();
		props_[EventKeyFieldName] = QVariant(key);

		QString evname = newitem->data(1, Qt::UserRole).toString();
		props_[EventNameFieldName] = QVariant(evname);

		emit completeChanged();
	}
}

void SelectEventWizardPage::doubleClicked(QTreeWidgetItem* item, int column)
{
	QString key = item->data(0, Qt::UserRole).toString();
	props_[EventKeyFieldName] = QVariant(key);

	QString evname = item->data(1, Qt::UserRole).toString();
	props_[EventNameFieldName] = QVariant(evname);

	emit completeChanged();

	QWizard* wiz = wizard();
	wiz->next();
}

bool SelectEventWizardPage::isComplete() const 
{
	auto it = props_.find(EventKeyFieldName);
	return it != props_.end();
}