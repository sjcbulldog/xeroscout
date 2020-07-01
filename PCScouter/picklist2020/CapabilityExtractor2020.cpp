#include "CapabilityExtractor2020.h"
#include "RobotCapabilities.h"
#include "Const2020.h"

CapabilityExtractor2020::CapabilityExtractor2020(const Headers& headers, const DataArray& data) : CapabilityExtractor(headers, data)
{
}

CapabilityExtractor2020::~CapabilityExtractor2020()
{
}

void CapabilityExtractor2020::createBallSet(const std::list<Result*>& res, std::vector<int>& set, int highcol, int lowcol)
{
	set.clear();

	for (const Result* r : res)
	{
		int balls = static_cast<int>(r->value(highcol) + r->value(lowcol) + 0.5);
		set.push_back(balls);
	}
}

double CapabilityExtractor2020::wheel(int team, const std::list<AllianceGroup*>& mat, int threshold, int auto_low, int auto_high, int teleop_low, int teleop_high, int spin)
{
	std::vector<std::pair<bool, bool>> tries;

	for (const AllianceGroup* gr : mat)
	{
		int made_auto = static_cast<int>(gr->sum(auto_low) + gr->sum(auto_high) + 0.5);
		int made_tele = static_cast<int>(gr->sum(teleop_low) + gr->sum(teleop_high) + 0.5);
		int toward = std::min(made_auto, 9) + made_tele;
		if (toward < threshold)
		{
			//
			// Did not hit the threshold, no opportunity to spin the wheel
			//
			tries.push_back(std::make_pair(false, false));
			continue;
		}

		if (gr->sum(spin) > 0.5)
		{
			//
			// Someone spun the wheel, was it us
			//
			if (testBoolean(gr->teamResult(team), spin))
			{
				//
				// It was us, give us credit
				//
				tries.push_back(std::make_pair(true, true));
			}
			else
			{
				//
				// It was not an opportunity, as our alliance partner spun the wheel
				//
				tries.push_back(std::make_pair(false, false));
			}
		}
		else {
			//
			// We reached the ball count, but did not spin
			//
			tries.push_back(std::make_pair(false, true));
		}
	}

	int opportunities = 0, successes = 0;

	for (auto pair : tries)
	{
		if (pair.second)
			opportunities++;

		if (pair.first)
			successes++;
	}

	if (opportunities == 0)
		return 0.0;

	return static_cast<double>(successes) / static_cast<double>(opportunities);
}

double CapabilityExtractor2020::assisted2(int team, const std::list<AllianceGroup*>& mat, int climbed, int climb_was_assisted, int climb_assists)
{
	std::vector<std::pair<bool, bool>> tries;

	for (const AllianceGroup* gr : mat)
	{
		int available = 0;
		for (size_t i = 0; i < 3; i++) {
			const Result* r = gr->result(i);
			if (r->team() != team && (!testBoolean(r, climbed) || testBoolean(r, climb_was_assisted)))
				available++;
		}

		const Result* tr = gr->teamResult(team);
		assert(tr != nullptr);
		int assisted = static_cast<int>(tr->value(climb_assists) + 0.5);

		tries.push_back(std::make_pair(available == 2, assisted == 2));
	}

	int opportunities = 0, successes = 0;

	for (auto pair : tries)
	{
		if (pair.first)
			opportunities++;

		if (pair.second)
			successes++;
	}

	if (opportunities == 0)
		return 0.0;

	return static_cast<double>(successes) / static_cast<double>(opportunities);
}

double CapabilityExtractor2020::assisted1(int team, const std::list<AllianceGroup*>& mat, int climbed, int climb_was_assisted, int climb_assists)
{
	std::vector<std::pair<int, bool>> tries;

	for (const AllianceGroup* gr : mat)
	{
		int available = 0;
		for (size_t i = 0; i < 3; i++) {
			const Result* r = gr->result(i);
			if (r->team() != team && (!testBoolean(r, climbed) || testBoolean(r, climb_was_assisted)))
				available++;
		}

		const Result* tr = gr->teamResult(team);
		assert(tr != nullptr);
		int assisted = static_cast<int>(tr->value(climb_assists) + 0.5);

		tries.push_back(std::make_pair(available, assisted == 1));
	}

	int opportunities = 0, successes = 0;

	for (auto pair : tries)
	{
		opportunities += pair.first;

		if (pair.second)
			successes++;
	}

	if (opportunities == 0)
		return 0.0;

	return static_cast<double>(successes) / static_cast<double>(opportunities);
}

double CapabilityExtractor2020::park(const std::list<Result*>& res, int climbed, int parked)
{
	double attempts = 0.0;
	double successes = 0.0;

	for (auto r : res) {
		if (testBoolean(r, climbed))
			continue;

		attempts += 1.0;
		if (testBoolean(r, parked))
			successes += 1.0;
	}

	if (attempts < 0.5)
		return 0.0;

	return successes / attempts;
}

double CapabilityExtractor2020::balance(const std::list<Result*>& res, int climbed, int climbed_was_assisted, int balance)
{
	double attempts = 0.0;
	double successes = 0.0;

	for (auto r : res) {
		if (testBoolean(r, climbed) && !testBoolean(r, climbed_was_assisted))
		{
			attempts += 1.0;
			if (testBoolean(r, balance))
			{
				successes += 1.0;
			}
		}
	}

	return successes / attempts;
}

bool CapabilityExtractor2020::createRobotCapabilities()
{
	createResults();
	createMatches();

	int auto_line_col = column("auto_line");
	int auto_low_col = column("auto_low");
	int auto_high_col = column("auto_high");
	int teleop_low_col = column("tele_low");
	int teleop_high_col = column("tele_high");
	int wheel_spin_col = column("wheel_spin");
	int wheel_color_col = column("wheel_color");
	int climbed_col = column("climbed");
	int climb_assists_col = column("climb_assists");
	int climb_was_assisted_col = column("climb_was_assisted");
	int park_col = column("park");
	int balanced_col = column("balanced");

	std::vector<int> climb_cols = { climb_assists_col, climb_was_assisted_col, park_col };
	std::vector<bool> climb_invs = { false, false, false };

	std::vector<int> climb_unassisted_cols = { climbed_col, climb_was_assisted_col };
	std::vector<bool> climb_unassisted_invs = { false, true };

	std::vector<bool> climb_assisted_invs = { false, false };

	for (int team : teams())
	{
		RobotCapabilities* robot = new RobotCapabilities(team);

		//
		// Get the list of results for this one team
		//
		auto res = results(team);
		auto mat = matches(team);

		double auto_high = average(res, auto_high_col);
		double teleop_high = average(res, teleop_high_col);

		double auto_outer = auto_high * (1 - Const2020::InnerOdds);
		double auto_inner = auto_high * Const2020::InnerOdds;
		double teleop_outer = teleop_high * (1 - Const2020::InnerOdds);
		double teleop_inner = teleop_high * Const2020::InnerOdds;

		auto endgamecb = std::bind(&CapabilityExtractor::logicalOR, std::placeholders::_1, climb_cols, climb_invs);
		double px_endgame = average(res, endgamecb);

		double time_conversion_factor = (Const2020::TeleopLength) / (Const2020::TeleopLength * (1 - px_endgame) + (Const2020::TeleopLength - Const2020::EndGameLength) * px_endgame);

		averageResultsValue(robot, res, "auto_line");
		averageResultsValue(robot, res, "auto_low");
		robot->addDouble("auto_outer", auto_outer);
		robot->addDouble("auto_inner", auto_inner);
		robot->addDouble("tele_low", time_conversion_factor * average(res, teleop_low_col));
		robot->addDouble("tele_inner", time_conversion_factor * teleop_inner);
		robot->addDouble("tele_outer", time_conversion_factor * teleop_outer);

		std::vector<int> set;

		createBallSet(res, set, auto_high_col, auto_low_col);
		robot->addDistribution("auto_ball_dist", Distribution(set.begin(), set.end()));

		set.clear();
		createBallSet(res, set, teleop_high_col, teleop_low_col);
		robot->addDistribution("tele_ball_dist", Distribution(set.begin(), set.end()));
		
		double spin = wheel(team, mat, Const2020::WheelTurnThreshold, auto_low_col, auto_high_col, teleop_low_col, teleop_high_col, wheel_spin_col);
		robot->addDouble("wheel_spin", spin);

		double color = wheel(team, mat, Const2020::WheelColorThreshold, auto_low_col, auto_high_col, teleop_low_col, teleop_high_col, wheel_color_col);
		robot->addDouble("wheel_color", color);

		auto climbunassistcb = std::bind(&CapabilityExtractor::logicalAND, std::placeholders::_1, climb_unassisted_cols, climb_unassisted_invs);
		robot->addDouble("climb_unassisted", average(res, climbunassistcb));

		robot->addDouble("assist2", assisted2(team, mat, climbed_col, climb_was_assisted_col, climb_assists_col));
		robot->addDouble("assist1", assisted1(team, mat, climbed_col, climb_was_assisted_col, climb_assists_col));

		auto climbassistcb = std::bind(&CapabilityExtractor::logicalAND, std::placeholders::_1, climb_unassisted_cols, climb_assisted_invs);
		robot->addDouble("climb_was_assisted", average(res, climbassistcb));

		robot->addDouble("park", park(res, climbed_col, park_col));
		robot->addDouble("balance", balance(res, climbed_col, climb_was_assisted_col, balanced_col));

		addRobot(team, robot);
	}

	return true;
}