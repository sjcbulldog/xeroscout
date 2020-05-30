#pragma once

#include "ApplicationController.h"
#include "ScoutingDataModel.h"

class AllTeamSummaryController : public ApplicationController
{
public:
	AllTeamSummaryController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::datamodel::ScoutingDataSet& ds);
	~AllTeamSummaryController();

	bool isDone() override;
	void run() override;

	bool providesProgress() {
		return true;
	}

	int percentDone() {
		return pcnt_;
	}

private:
	void computeOneTeam(const QString& key);

private:
	xero::scouting::datamodel::ScoutingDataSet& ds_;
	int pcnt_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QStringList keys_;
	int index_;
	bool headers_;
};

