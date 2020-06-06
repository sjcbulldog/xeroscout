#pragma once

#include "PickListView.h"
#include "ApplicationController.h"
#include "PickListGenerator.h"
#include <QElapsedTimer>

class PickListController : public ApplicationController
{
public:
	PickListController(std::shared_ptr<xero::ba::BlueAlliance> ba, int team, int year,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::views::PickListView* view);
	~PickListController();

	bool isDone() override;
	void run() override;

	bool providesProgress() {
		return true;
	}

	int percentDone() {
		return pcnt_;
	}

	QString htmlPicklist() const {
		return gen_->picklist();
	}

	QString htmlRobotCapabilities() const {
		return gen_->robotCapabilities();
	}

private:
	void computeOneTeam(const QString& key);
	void logGenMessage(const QString& msg);

private:
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	int pcnt_;
	PickListGenerator* gen_;
	xero::scouting::views::PickListView* view_;
	QElapsedTimer timer_;
	bool started_;
};

