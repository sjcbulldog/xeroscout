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
#include <QListWidget>

class TabletPoolListWidget;
class AdvListWidget;

class TabletPoolWizardPage : public QWizardPage
{
public:
	TabletPoolWizardPage(NewEventBlueAllianceWizard::PropertyMap& props);
	virtual ~TabletPoolWizardPage();

	void setTabletList(const QStringList& list);

	bool isComplete() const override;

private:
	QWidget* createTabletPoolWidget(QWidget *parent);
	QWidget* createPitScoutList(QWidget* parent);
	QWidget* createMatchScoutList(QWidget* parent);

private:
	void poolChanged(QListWidgetItem* item);
	void matchChanged(QListWidgetItem* item);
	void pitChanged(QListWidgetItem* item);

	void tabletDeleted(const QString& name);
	void tabletRenamed(const QString& oldname, const QString& newname);

private:
	NewEventBlueAllianceWizard::PropertyMap& props_;
	TabletPoolListWidget* pool_list_;
	AdvListWidget* match_list_;
	AdvListWidget* pit_list_;
};

