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

#include "SelectEventWizardPage.h"
#include "BlueAllianceEngine.h"

#include <QtCore/QDebug>
#include <QtWidgets/QLabel>

using namespace xero::ba;

SelectEventWizardPage::SelectEventWizardPage(NewEventBlueAllianceWizard::PropertyMap &props, BlueAllianceEngine &engine) : props_(props), engine_(engine)
{
	QStringList headers = { "District", "Date", "Event" };

	setTitle("Select Event");

	holder_ = new QWidget();
	holder_layout_ = new QVBoxLayout();
	holder_->setLayout(holder_layout_);

	filter_widget_ = new QWidget();
	filter_layout_ = new QHBoxLayout();
	filter_widget_->setLayout(filter_layout_);

	QLabel* label = new QLabel("Filter: ");
	filter_layout_->addWidget(label);
	filter_ = new QLineEdit();
	filter_layout_->addWidget(filter_);

	holder_layout_->addWidget(filter_widget_);

	tree_ = new QTreeWidget();
	tree_->setColumnCount(3);
	tree_->setHeaderLabels(headers);

	filterList("");

	holder_layout_->addWidget(tree_);
	setLayout(holder_layout_);

	(void)connect(filter_, &QLineEdit::textChanged, this, &SelectEventWizardPage::filterList);

	(void)connect(tree_, &QTreeWidget::currentItemChanged, this, &SelectEventWizardPage::eventChanged);
	(void)connect(tree_, &QTreeWidget::itemDoubleClicked, this, &SelectEventWizardPage::doubleClicked);
}

SelectEventWizardPage::~SelectEventWizardPage()
{
}

void SelectEventWizardPage::filterList(const QString& text)
{
	auto& evs = engine_.events();

	tree_->clear();

	for (auto& pair : evs) {
		if (text.isEmpty() || pair.second->name().toLower().contains(text.toLower())) {
			QTreeWidgetItem* item = new QTreeWidgetItem(tree_);
			if (pair.second->district() != nullptr)
				item->setText(0, pair.second->district()->name());
			item->setText(1, pair.second->start().toString() + " - " + pair.second->end().toString());
			item->setText(2, pair.second->name());

			item->setData(0, Qt::UserRole, pair.first);
			item->setData(1, Qt::UserRole, pair.second->name());

			tree_->addTopLevelItem(item);
		}
	}

	tree_->resizeColumnToContents(0);
	tree_->resizeColumnToContents(1);
	tree_->resizeColumnToContents(2);

	tree_->sortByColumn(0, Qt::AscendingOrder);
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