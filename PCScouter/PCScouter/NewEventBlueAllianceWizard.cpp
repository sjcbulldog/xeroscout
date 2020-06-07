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



#include "NewEventBlueAllianceWizard.h"
#include "BlueAllianceEngine.h"
#include "SelectEventWizardPage.h"
#include "SelectScoutingFormsWizardPage.h"
#include "TabletPoolWizardPage.h"

using namespace xero::ba;

NewEventBlueAllianceWizard::NewEventBlueAllianceWizard(BlueAllianceEngine &engine)
{
	addPage(new SelectEventWizardPage(props_, engine));

	addPage(new SelectScoutingFormsWizardPage(props_));

	page_ = new TabletPoolWizardPage(props_);
	addPage(page_);
}

NewEventBlueAllianceWizard::~NewEventBlueAllianceWizard()
{
}

void NewEventBlueAllianceWizard::setTabletList(const QStringList& list)
{
	props_["tablets"] = QVariant(list);
	page_->setTabletList(list);
}

QStringList NewEventBlueAllianceWizard::getTabletList()
{
	QStringList list;

	auto it = props_.find("tablets");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}

QStringList NewEventBlueAllianceWizard::getPitTabletList()
{
	QStringList list;

	auto it = props_.find("pits");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}


QStringList NewEventBlueAllianceWizard::getMatchTabletList()
{
	QStringList list;

	auto it = props_.find("matches");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}


QString NewEventBlueAllianceWizard::getEventKey()
{
	QString result;

	auto it = props_.find(SelectEventWizardPage::EventKeyFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventBlueAllianceWizard::getEventName()
{
	QString result;

	auto it = props_.find(SelectEventWizardPage::EventNameFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventBlueAllianceWizard::getPitScoutingForm()
{
	QString result;

	auto it = props_.find(SelectScoutingFormsWizardPage::PitScoutFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventBlueAllianceWizard::getMatchScoutingForm()
{
	QString result;

	auto it = props_.find(SelectScoutingFormsWizardPage::MatchScoutFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}
