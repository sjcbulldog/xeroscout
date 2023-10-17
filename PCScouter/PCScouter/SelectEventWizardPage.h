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

#pragma once

#include <BlueAllianceEngine.h>
#include "NewEventBlueAllianceWizard.h"
#include <QtWidgets/QWizard>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLineEdit>

class SelectEventWizardPage : public QWizardPage
{
public:
	SelectEventWizardPage(NewEventBlueAllianceWizard::PropertyMap &map, xero::ba::BlueAllianceEngine &engine);
	virtual ~SelectEventWizardPage();

	bool isComplete() const override;

public:
	static constexpr const char* EventKeyFieldName = "key";
	static constexpr const char* EventNameFieldName = "name";

private:
	void eventChanged(QTreeWidgetItem* newitem, QTreeWidgetItem* olditem);
	void doubleClicked(QTreeWidgetItem* item, int column);

	void filterList(const QString &text);

private:
	QVBoxLayout* holder_layout_;
	QWidget* holder_;
	QWidget* filter_widget_;
	QHBoxLayout* filter_layout_;
	QLineEdit* filter_;
	QTreeWidget *tree_;
	NewEventBlueAllianceWizard::PropertyMap& props_;
	xero::ba::BlueAllianceEngine& engine_;
};

