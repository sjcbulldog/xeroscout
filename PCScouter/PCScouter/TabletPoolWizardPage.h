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

