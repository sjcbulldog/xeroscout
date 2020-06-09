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

#include "NewEventBlueAllianceWizard.h"
#include <QWizard>
#include <QTreeWidget>

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

private:
	NewEventBlueAllianceWizard::PropertyMap& props_;
};

