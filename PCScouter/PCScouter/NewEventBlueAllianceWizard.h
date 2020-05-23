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

#pragma once

#include "BlueAllianceEngine.h"
#include <QWizard>
#include <QString>
#include <QVariant>
#include <map>

class TabletPoolWizardPage;

class NewEventBlueAllianceWizard : public QWizard
{
	Q_OBJECT;

public:
	typedef std::map<QString, QVariant> PropertyMap;

public:
	NewEventBlueAllianceWizard(xero::ba::BlueAllianceEngine &engine);
	virtual ~NewEventBlueAllianceWizard();

	QString getEventKey();
	QString getEventName();
	QString getPitScoutingForm();
	QString getMatchScoutingForm();

	void setTabletList(const QStringList& tablets);
	QStringList getTabletList();

	QStringList getPitTabletList();
	QStringList getMatchTabletList();

private:
	PropertyMap props_;
	TabletPoolWizardPage* page_;
};

