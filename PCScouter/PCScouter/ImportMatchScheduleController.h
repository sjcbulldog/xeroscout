#pragma once

#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include <QStringList>

class ImportMatchScheduleController : public ApplicationController
{
public:
	ImportMatchScheduleController(std::shared_ptr<xero::ba::BlueAlliance> ba, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	virtual ~ImportMatchScheduleController();

	bool isDone() override;
	void run() override;

private:
	enum class State
	{
		Start,
		WaitingForEvents,
		WaitingForMatches,
		WaitingForTeams,
		NoMatches,
		Done,
		Error,
	};

private:
	void start();
	void gotMatches();
	void gotTeams();
	void noMatches();

private:
	State state_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QStringList team_keys_;
};
