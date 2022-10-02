#include "NewEventOffseasonWizard.h"
#include "SelectScoutingFormsWizardPage.h"
#include "SelectOffseasonRosterPage.h"
#include "TabletPoolWizardPage.h"

NewEventOffseasonWizard::NewEventOffseasonWizard(xero::scouting::datamodel::ImageManager& mgr, const QStringList &tablets)
{
	tablets_ = tablets;
	addPage(new SelectOffseasonRosterPage(props_));
	addPage(new SelectScoutingFormsWizardPage(props_));

	page_ = new TabletPoolWizardPage(props_);
	addPage(page_);
}

NewEventOffseasonWizard::~NewEventOffseasonWizard()
{
}

QString NewEventOffseasonWizard::getRosterForm()
{
	QString result;

	auto it = props_.find(SelectOffseasonRosterPage::RosterFileName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventOffseasonWizard::getEventName()
{
	QString result;

	auto it = props_.find(SelectOffseasonRosterPage::EventTitleName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventOffseasonWizard::getEventKey()
{
	QString result;

	auto it = props_.find(SelectOffseasonRosterPage::EventKeyName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventOffseasonWizard::getPitScoutingForm()
{
	QString result;

	auto it = props_.find(SelectScoutingFormsWizardPage::PitScoutFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}

QString NewEventOffseasonWizard::getMatchScoutingForm()
{
	QString result;

	auto it = props_.find(SelectScoutingFormsWizardPage::MatchScoutFieldName);
	if (it != props_.end())
		result = it->second.toString();

	return result;
}


QStringList NewEventOffseasonWizard::getTabletList()
{
	QStringList list;

	auto it = props_.find("tablets");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}

QStringList NewEventOffseasonWizard::getPitTabletList()
{
	QStringList list;

	auto it = props_.find("pits");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}


QStringList NewEventOffseasonWizard::getMatchTabletList()
{
	QStringList list;

	auto it = props_.find("matches");
	if (it != props_.end())
		list = it->second.toStringList();

	return list;
}


void NewEventOffseasonWizard::setTitle()
{
	setWindowTitle(getEventName());
}