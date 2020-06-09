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
