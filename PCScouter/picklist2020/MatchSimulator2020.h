#pragma once

#include "MatchSimulator.h"
#include "ClimbingStrategies.h"
#include <list>
#include <vector>

class MatchAlliance;
class RobotCapabilities;

class MatchSimulator2020 : public MatchSimulator
{
public:
	MatchSimulator2020();
	virtual ~MatchSimulator2020();

	double simulate(const MatchAlliance& alliance);

private:
	double simulate(const MatchAlliance& alliance, const std::vector<ClimbingStrategy>& climbing);

	double scale(int index, RobotCapabilities* robot, const std::vector<ClimbingStrategy>& climbing);
	double climbPoints(const MatchAlliance& alliance, const std::vector<ClimbingStrategy>& climbing);

	std::pair<double, double> bonusBallsHit(const MatchAlliance& alliancs);

	static void createClimbingStrategies();
	static void findColumns(const RobotCapabilities* robot);

	static double geomean(const std::vector<double>& data);

private:
	double climb_scale_;

	static std::list<std::vector<ClimbingStrategy>> climbing_strategies_;
	static int auto_line_col_;
	static int auto_low_col_;
	static int auto_outer_col_;
	static int auto_inner_col_;
	static int tele_low_col_;
	static int tele_inner_col_;
	static int tele_outer_col_;
	static int wheel_spin_col_;
	static int wheel_color_col_;
	static int climb_unassisted_col_;
	static int assist2_col_;
	static int assist1_col_;
	static int climb_was_assisted_col_;
	static int park_col_;
	static int balance_col_;
	static int auto_ball_dist_col_;
	static int tele_ball_dist_col_;
};

