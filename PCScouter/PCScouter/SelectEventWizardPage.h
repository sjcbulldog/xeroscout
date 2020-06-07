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

