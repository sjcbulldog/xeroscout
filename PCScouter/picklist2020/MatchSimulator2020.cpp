#include "MatchSimulator2020.h"
#include "MatchAlliance.h"
#include "RobotCapabilities.h"
#include "Const2020.h"
#include <algorithm>
#include <cassert>
#include <iostream>

std::list<std::vector<ClimbingStrategy>> MatchSimulator2020::climbing_strategies_;
int MatchSimulator2020::auto_line_col_;
int MatchSimulator2020::auto_low_col_;
int MatchSimulator2020::auto_outer_col_;
int MatchSimulator2020::auto_inner_col_;
int MatchSimulator2020::tele_low_col_;
int MatchSimulator2020::tele_inner_col_;
int MatchSimulator2020::tele_outer_col_;
int MatchSimulator2020::wheel_spin_col_;
int MatchSimulator2020::wheel_color_col_;
int MatchSimulator2020::climb_unassisted_col_;
int MatchSimulator2020::assist2_col_;
int MatchSimulator2020::assist1_col_;
int MatchSimulator2020::climb_was_assisted_col_;
int MatchSimulator2020::park_col_;
int MatchSimulator2020::balance_col_;
int MatchSimulator2020::auto_ball_dist_col_;
int MatchSimulator2020::tele_ball_dist_col_;

MatchSimulator2020::MatchSimulator2020()
{
	climb_scale_ = (Const2020::TeleopLength - Const2020::EndGameLength) / Const2020::TeleopLength;
}

MatchSimulator2020::~MatchSimulator2020()
{
}

double MatchSimulator2020::geomean(const std::vector<double>& data)
{
	double p = 1.0;
	for (double d : data)
		p *= d;
	return pow(p, 1.0 / data.size());
}

void MatchSimulator2020::createClimbingStrategies()
{
	climbing_strategies_.clear();

	for (ClimbingStrategy r1 : AllClimbingStrategies)
	{
		for (ClimbingStrategy r2 : AllClimbingStrategies)
		{
			for (ClimbingStrategy r3 : AllClimbingStrategies)
			{
				std::vector<ClimbingStrategy> strategy = { r1, r2, r3 };
				climbing_strategies_.push_back(strategy);
			}
		}
	}
}

void MatchSimulator2020::findColumns(const RobotCapabilities* robot)
{
	assert(robot != nullptr);

	auto_line_col_ = robot->doubleColumn("auto_line");
	assert(auto_line_col_ != -1);

	auto_low_col_ = robot->doubleColumn("auto_low");
	assert(auto_line_col_ != -1);

	auto_outer_col_ = robot->doubleColumn("auto_outer");
	assert(auto_line_col_ != -1);

	auto_inner_col_ = robot->doubleColumn("auto_inner");
	assert(auto_line_col_ != -1);

	tele_low_col_ = robot->doubleColumn("tele_low");
	assert(auto_line_col_ != -1);

	tele_inner_col_ = robot->doubleColumn("tele_inner");
	assert(auto_line_col_ != -1);

	tele_outer_col_ = robot->doubleColumn("tele_outer");
	assert(auto_line_col_ != -1);

	wheel_spin_col_ = robot->doubleColumn("wheel_spin");
	assert(auto_line_col_ != -1);

	wheel_color_col_ = robot->doubleColumn("wheel_color");
	assert(auto_line_col_ != -1);

	climb_unassisted_col_ = robot->doubleColumn("climb_unassisted");
	assert(auto_line_col_ != -1);

	assist2_col_ = robot->doubleColumn("assist2");
	assert(auto_line_col_ != -1);

	assist1_col_ = robot->doubleColumn("assist1");
	assert(auto_line_col_ != -1);

	climb_was_assisted_col_ = robot->doubleColumn("climb_was_assisted");
	assert(auto_line_col_ != -1);

	park_col_ = robot->doubleColumn("park");
	assert(auto_line_col_ != -1);

	balance_col_ = robot->doubleColumn("balance");
	assert(auto_line_col_ != -1);

	auto_ball_dist_col_ = robot->distColumn("auto_ball_dist");
	assert(auto_line_col_ != -1);

	tele_ball_dist_col_ = robot->distColumn("tele_ball_dist");
	assert(auto_line_col_ != -1);
}

double MatchSimulator2020::simulate(const MatchAlliance& alliance)
{
	if (climbing_strategies_.size() == 0)
	{
		createClimbingStrategies();
		findColumns(alliance.robot(0));
	}

	double score = 0;
	for (const std::vector<ClimbingStrategy>& strategy : climbing_strategies_)
	{
		double d = simulate(alliance, strategy);
		score = std::max(d, score);
	}

	return score;
}

double MatchSimulator2020::scale(int index, const RobotCapabilities* robot, const std::vector<ClimbingStrategy>& climbing)
{
	assert(index < climbing.size());
	if (climbing[index] == ClimbingStrategy::None)
		return 1.0;

	return climb_scale_;
}

std::pair<double, double> MatchSimulator2020::bonusBallsHit(const MatchAlliance& alliance)
{
	Distribution auto_balls = alliance.sumDistribution(auto_ball_dist_col_);
	auto_balls.assertValid();

	Distribution auto_balls_capped = auto_balls.capAt(9);
	auto_balls_capped.assertValid();

	Distribution tele_balls = alliance.sumDistribution(tele_ball_dist_col_);
	tele_balls.assertValid();

	Distribution total = auto_balls_capped + tele_balls;
	total.assertValid();

	double p1 = total.chances(29);
	double p2 = total.chances(49);
	return std::make_pair(p1, p2);
}

double MatchSimulator2020::climbPoints(const MatchAlliance& alliance, const std::vector<ClimbingStrategy>& climbing)
{
	double score = 0;
	double climbed = 0;
	std::vector<double> balancers;

	for (size_t i = 0; i < 3 ; i++)
	{
		const RobotCapabilities* r = alliance.robot(i);
		if (r == nullptr)
			continue;

		switch (climbing[i])
		{
		case ClimbingStrategy::None:
			break;
		case ClimbingStrategy::Park:
			score += Const2020::ParkPoints * r->doubleValue(park_col_);
			break;
		case ClimbingStrategy::Self:
			climbed += r->doubleValue(climb_unassisted_col_);
			balancers.push_back(r->doubleValue(balance_col_));
			break;
		case ClimbingStrategy::Assisted:
			break;
		}
	}

	if (balancers.size() > 0)
		score += Const2020::BalancePoints * geomean(balancers);

	score += climbed * Const2020::ClimbPoints;

	return score;
}

double MatchSimulator2020::simulate(const MatchAlliance& alliance, const std::vector<ClimbingStrategy>& climbing)
{
	double score = 0.0;

	double autoline = Const2020::AutoLinePoints * alliance.sum(auto_line_col_);
	double autolow = Const2020::AutoBallLowPoints * alliance.sum(auto_low_col_);
	double autoouter = Const2020::AutoBallOuterPoints * alliance.sum(auto_outer_col_);
	double autoinner = Const2020::AutoBallInnerPoints * alliance.sum(auto_inner_col_);

	auto cb = std::bind(&MatchSimulator2020::scale, this, std::placeholders::_1, std::placeholders::_2, climbing);
	double telelow = Const2020::TeleBallLowPoints* alliance.sum(tele_low_col_, cb);
	double teleouter = Const2020::TeleBallOuterPoints * alliance.sum(tele_outer_col_, cb);
	double teleinner = Const2020::TeleBallInnerPoints * alliance.sum(tele_inner_col_, cb);

	auto pcnt = bonusBallsHit(alliance);

	double spinmax = alliance.max(wheel_spin_col_);
	double wheelspin = Const2020::WheelSpinPoints * spinmax * pcnt.first;

	double colormax = alliance.max(wheel_color_col_);
	double wheelcolor = Const2020::WheelColorPoints * colormax * pcnt.second;

	double climb = climbPoints(alliance, climbing);

	return autoline + autolow + autoouter + autoinner + telelow + teleouter + teleinner + wheelspin + wheelcolor + climb;
}