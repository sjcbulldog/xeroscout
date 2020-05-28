#pragma once
#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataSet.h"

class TeamSummaryController : public ApplicationController
{
public:
	TeamSummaryController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::datamodel::ScoutingDataSet& ds);
	virtual ~TeamSummaryController();

	bool isDone() override;
	void run() override;

	virtual bool providesProgress() { return true; }
	virtual int percentDone() { return pcnt_; }

private:
	void progress(int pcnt);

private:
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	// xero::scouting::datamodel::AllTeamSummaryGenerator* gen_;
	int pcnt_;
	bool done_;
};

