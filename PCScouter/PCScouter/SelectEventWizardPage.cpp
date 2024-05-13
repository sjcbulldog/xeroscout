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
#include <QtWidgets/QSpinBox>

using namespace xero::ba;

SelectEventWizardPage::SelectEventWizardPage(NewEventBlueAllianceWizard::PropertyMap &props, BlueAllianceEngine &engine) : props_(props), engine_(engine)
{
	QStringList headers = { "District", "Date", "Event" };

	setTitle("Select Event");

	holder_ = new QWidget();
	holder_layout_ = new QVBoxLayout();
	holder_->setLayout(holder_layout_);

	QHBoxLayout *rowlayout = new QHBoxLayout();
	QWidget *row = new QWidget(this);
	row->setLayout(rowlayout);
	holder_layout_->addWidget(row);
	QLabel *l = new QLabel(row);
	l->setText("Event Year: ");
	rowlayout->addWidget(l);
	year_widget_ = new QSpinBox(row);
	year_widget_->setMinimum(2004);
	year_widget_->setMaximum(2200);
	current_year_ = QDateTime::currentDateTime().date().year();
	year_widget_->setValue(current_year_);
	rowlayout->addWidget(year_widget_);
	(void)connect(year_widget_, &QSpinBox::valueChanged, this, &SelectEventWizardPage::yearChanged);

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

	holder_layout_->addWidget(tree_);
	setLayout(holder_layout_);

	(void)connect(filter_, &QLineEdit::textChanged, this, &SelectEventWizardPage::filterList);

	(void)connect(tree_, &QTreeWidget::currentItemChanged, this, &SelectEventWizardPage::eventChanged);
	(void)connect(tree_, &QTreeWidget::itemDoubleClicked, this, &SelectEventWizardPage::doubleClicked);
	(void)connect(tree_, &QTreeWidget::itemExpanded, this, &SelectEventWizardPage::itemExpanded);

	tree_->setDisabled(true);
	fetchEvents();
}

SelectEventWizardPage::~SelectEventWizardPage()
{
}

void SelectEventWizardPage::filterList(const QString& text)
{
	auto& evs = engine_.events();
	QMap<QString, QTreeWidgetItem*> districts;

	tree_->clear();

	for (auto& pair : evs) {
		if (text.isEmpty() || pair.second->name().toLower().contains(text.toLower())) {
			if (pair.second->year() == current_year_) {
				QString diststr = (pair.second->district() != nullptr) ? pair.second->district()->name() : "<None>";

				QTreeWidgetItem* distitem;
				if (districts.contains(diststr)) {
					distitem = districts.value(diststr);
				}
				else {
					distitem = new QTreeWidgetItem(tree_);
					distitem->setText(0, diststr);
					districts.insert(diststr, distitem);
					tree_->addTopLevelItem(distitem);
				}

				QTreeWidgetItem* item = new QTreeWidgetItem();
				item->setText(1, pair.second->start().toString() + " - " + pair.second->end().toString());
				item->setText(2, pair.second->name());

				item->setData(0, Qt::UserRole, pair.first);
				item->setData(1, Qt::UserRole, pair.second->name());
				distitem->addChild(item);
			}
		}
	}

	tree_->resizeColumnToContents(0);
	tree_->resizeColumnToContents(1);
	tree_->resizeColumnToContents(2);

	tree_->sortByColumn(0, Qt::AscendingOrder);
}

void SelectEventWizardPage::itemExpanded(QTreeWidgetItem* newitem)
{
	tree_->resizeColumnToContents(0);
	tree_->resizeColumnToContents(1);
	tree_->resizeColumnToContents(2);
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
	if (key.length() > 0) {
		props_[EventKeyFieldName] = QVariant(key);

		QString evname = item->data(1, Qt::UserRole).toString();
		props_[EventNameFieldName] = QVariant(evname);

		emit completeChanged();

		QWizard* wiz = wizard();
		wiz->next();
	}
}

bool SelectEventWizardPage::isComplete() const 
{
	auto it = props_.find(EventKeyFieldName);
	return it != props_.end();
}

void SelectEventWizardPage::fetchEvents()
{
	year_widget_->setDisabled(true);
	tree_->setDisabled(true);
	(void)connect(&engine_, &BlueAllianceEngine::done, this, &SelectEventWizardPage::done);
	engine_.requestEvents(current_year_);
}

void SelectEventWizardPage::done()
{
	(void)disconnect(&engine_, &BlueAllianceEngine::done, this, &SelectEventWizardPage::done);
	year_widget_->setDisabled(false);
	tree_->setDisabled(false);
	filterList("");
}

void SelectEventWizardPage::yearChanged(int year)
{
	if (year > 2004 && year < 3000) {
		props_[YearName] = QVariant(year);
		current_year_ = year;
		fetchEvents();
	}
}
