#pragma once

#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include "ImageManager.h"
#include <QStringList>
#include <QSettings>

class NewOffseasonEventAppController :  public ApplicationController
{
public:
	NewOffseasonEventAppController(xero::scouting::datamodel::ImageManager& mgr, const QStringList& tablets, int year);
	virtual ~NewOffseasonEventAppController();

	bool isDone() override;
	void run() override;

	const QStringList& tablets() {
		return tablets_;
	}

	virtual bool shouldDisableApp();

private:
	enum class State
	{
		Start,
		NeedTeams,
		NeedMatches,
		Done,
		Error
	};

	void promptUser();
	bool addTeamFromRoster(QString roster);

private:
	int year_;
	State state_;
	QStringList tablets_;
	QStringList team_keys_;
	xero::scouting::datamodel::ImageManager& images_;
};

