#pragma once
#include "ApplicationController.h"
#include "ScoutingDataModel.h"

class ImportMatchDataController : public ApplicationController
{
public:
	ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	virtual ~ImportMatchDataController();

	bool isDone() override;
	void run() override;

private:
	enum class State
	{
		Start,
		Done,
		Error,
		WaitingForMatches,
		WaitingForDetail,
		WaitingForTeams,
		WaitingForRankings
	};

private:
	void gotDetail();
	void gotRankings();

private:
	State state_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
};

