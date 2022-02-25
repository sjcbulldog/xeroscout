#pragma once

#include "ScoutingDataModel.h"
#include <vector>
#include <array>

class MatchScheduleGenerator
{
public:
	MatchScheduleGenerator(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model, int count) {
		model_ = model;
		matches_.resize(count);
	}

	void generate();

private:
	void createInitialSchedule();
	void pertubate();
	double computeScore();

private:
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model_;
	std::vector<int> teams_;

	std::vector<std::array<int, 6>> matches_;
};

