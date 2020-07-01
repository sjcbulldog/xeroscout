#include "TeamPicker2020.h"
#include "MatchSimulator2020.h"
#include "CapabilityExtractor2020.h"
#include "Const2020.h"
#include "MatchAlliance.h"
#include "Pick.h"
#include <iostream>

std::list<std::string> TeamPicker2020::required_ =
{
	"auto_line",
	"auto_low",
	"auto_high",
	"tele_low",
	"tele_high",
	"wheel_spin",
	"wheel_color",
	"climbed",
	"climb_assists",
	"climb_was_assisted",
	"park",
	"balanced"
};

TeamPicker2020::TeamPicker2020(const Headers& headers, const DataArray& data) : TeamPicker(headers), extractor_(headers, data), third_robot_(0)
{
	noisy_ = false;
}

TeamPicker2020::~TeamPicker2020()
{
}

bool TeamPicker2020::isDataOk()
{
	if (!TeamPicker::isDataOk())
		return false;

	return checkColumns(required_);
}

void TeamPicker2020::generateThirdRobot(const std::list<int> &teams, const std::vector<std::pair<int, double>> &scores)
{
	std::vector<const RobotCapabilities*> third;
	if (teams.size() < 27)
	{
		// Use the lowest five ranking robots
		for (size_t i = teams.size() - 5; i < teams.size(); i++)
			third.push_back(extractor_.robot(scores[i].first));
	}
	else
	{
		// Use robots 22 - 24
		third.push_back(extractor_.robot(scores[22].first));
		third.push_back(extractor_.robot(scores[23].first));
		third.push_back(extractor_.robot(scores[24].first));
		third.push_back(extractor_.robot(scores[25].first));
		third.push_back(extractor_.robot(scores[26].first));
	}

	// Team # 100000 is the pseudo capabilites of a third robot
	third_robot_ = RobotCapabilities::mean(100000, third.begin(), third.end());
}

std::vector<std::pair<int, double>> TeamPicker2020::generateTwoTeamScores(int team, const std::list<int>& teams)
{
	std::vector<std::pair<int, double>> scores;

	for (auto pick : teams)
	{
		MatchAlliance ma(std::vector<RobotCapabilities*> { extractor_.robot(team), extractor_.robot(pick) });

		if (noisy_)
		{
			std::cout << "Simulating (Two Robots): " << ma.robot(0)->team() << " " << ma.robot(1)->team() << " ... ";
			std::cout.flush();
		}

		double score = simulator_.simulate(ma);
		scores.push_back(std::make_pair(pick, score));

		if (noisy_)
		{
			std::cout << score;
			std::cout << std::endl;
		}
	}

	std::sort(scores.begin(), scores.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool { return a.second > b.second; });

	return scores;
}

std::vector<std::pair<int, double>> TeamPicker2020::generateThreeTeamScores(int team, const std::list<int>& teams)
{
	std::vector<std::pair<int, double>> scores;

	for (auto pick : teams)
	{
		MatchAlliance ma(std::vector<RobotCapabilities*> { extractor_.robot(team), extractor_.robot(pick), &third_robot_ });

		if (noisy_)
		{
			std::cout << "Simulating (Three Robots): " << ma.robot(0)->team() << " " << ma.robot(1)->team() << " ... ";
			std::cout.flush();
		}

		double score = simulator_.simulate(ma);
		scores.push_back(std::make_pair(pick, score));

		if (noisy_)
		{
			std::cout << score;
			std::cout << std::endl;
		}
	}

	std::sort(scores.begin(), scores.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool { return a.second > b.second; });

	return scores;
}

bool TeamPicker2020::createPickList(int team)
{
	//
	// Make sure the data contains the columns we expect
	//
	if (!isDataOk())
		return false;

	//
	// Extract robot capabilities from the data
	//
	if (!extractor_.createRobotCapabilities())
		return false;

	//
	// Create the team list that excludes the one target team
	//
	std::list<int> teams = extractor_.teams();
	auto it = std::find(teams.begin(), teams.end(), team);
	if (it == teams.end())
	{
		std::cerr << "TeamPicker2020: the target team " << team << " was not in the loaded data" << std::endl;
		return 1;
	}
	teams.erase(it);

	//
	// Rank the other robots based on how well an alliance of two robots would do, the target robot
	// and a second robot.
	//
	auto scores2 = generateTwoTeamScores(team, teams);

	//
	// Look down at the approximately 22nd through 27th ranked robots and create a composite
	// robot that is the mean of these
	//
	generateThirdRobot(teams, scores2);

	//
	// Simulate the target robot and each of the other robots, with a representative third robot.  This
	// generates the first robot pick list
	//
	auto firstpick = generateThreeTeamScores(team, teams);

	//
	// Now, generate the complete pick list
	//
	for (auto pair : firstpick)
	{
		Pick pick(pair.first, pair.second);

		//
		// Create a list without the target team and without the first pick we are processing
		//
		std::list<int> others = teams;
		auto it = std::find(others.begin(), others.end(), pair.first);
		assert(it != others.end());
		others.erase(it);

		for (auto third : others)
		{
			if (noisy_)
			{
				std::cout << "Simulating " << team << " " << pair.first << " " << third << " ... ";
			}

			MatchAlliance ma(std::vector<RobotCapabilities*> { extractor_.robot(team), extractor_.robot(pair.first), extractor_.robot(third) });
			double score = simulator_.simulate(ma);
			pick.addScore(third, score);

			if (noisy_)
			{
				std::cout << score << std::endl;
			}
		}

		pick.sort();
		picks_.push_back(pick);
	}

	return true;
}