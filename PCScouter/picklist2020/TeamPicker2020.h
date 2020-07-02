#pragma once

#include "CapabilityExtractor2020.h"
#include "MatchSimulator2020.h"
#include "ClimbingStrategies.h"
#include "TeamPicker.h"
#include "Pick.h"
#include <cmath>

class TeamPicker2020 : public TeamPicker
{
public:
	TeamPicker2020(const Headers& headers, const DataArray& data);
	virtual ~TeamPicker2020();

	bool createPickList(int team) override ;
	void setNoisy(bool b) { noisy_ = b; }

	const std::vector<Pick> &picks() const {
		return picks_;
	}

	std::list<const RobotCapabilities*> robots() {
		return extractor_.robots();
	}

protected:
	bool isDataOk() override;

	std::vector<std::pair<int, double>> generateTwoTeamScores(int team, const std::list<int>& teams);
	std::vector<std::pair<int, double>> generateThreeTeamScores(int team, const std::list<int>& teams);

	void generateThirdRobot(const std::list<int>& teams, const std::vector<std::pair<int, double>>& scores);

private:
	CapabilityExtractor2020 extractor_;
	MatchSimulator2020 simulator_;

private:

	// The "representative" third robot
	RobotCapabilities third_robot_;

	std::vector<Pick> picks_;

	bool noisy_;

	static std::list<std::string> required_;
};
