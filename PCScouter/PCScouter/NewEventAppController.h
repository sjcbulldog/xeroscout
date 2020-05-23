#pragma once

#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include <QStringList>

class NewEventAppController : public ApplicationController
{
public:
	NewEventAppController(std::shared_ptr<xero::ba::BlueAlliance> ba, const QStringList &tablets, int year);
	virtual ~NewEventAppController();

	bool isDone() override;
	void run() override;

	const QStringList& tablets() {
		return tablets_;
	}

	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dataModel() {
		return dm_;
	}

private:
	enum class State
	{
		Start,
		WaitingForEvents,
		WaitingForMatches,
		WaitingForTeams,
		Done,
		Error,
	};

private:
	void promptUser();
	void start();
	void gotMatches();
	void gotTeams();

private:	
	int year_;
	State state_;
	QStringList tablets_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QStringList team_keys_;
};
