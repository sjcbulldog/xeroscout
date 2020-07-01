#include "RobotCapabilities.h"

std::map<std::string, int> RobotCapabilities::double_columns_;
std::map<std::string, int> RobotCapabilities::dist_columns_;

RobotCapabilities::RobotCapabilities(int team)
{
	team_ = team;
}

RobotCapabilities::~RobotCapabilities()
{
}

void RobotCapabilities::addDouble(const std::string& name, double value)
{
	int col = doubleColumn(name);
	if (col == -1)
	{
		col = static_cast<int>(double_columns_.size());
		double_columns_.insert_or_assign(name, col);
	}

	if (values_.size() <= col)
		values_.resize(col + 1);

	values_[col] = value;
}

void RobotCapabilities::addDistribution(const std::string& name, const Distribution &dist)
{
	int col = distColumn(name);
	if (col == -1)
	{
		col = static_cast<int>(dist_columns_.size());
		dist_columns_.insert_or_assign(name, col);
	}

	if (dists_.size() <= col)
		dists_.resize(col + 1);

	dists_[col] = dist;
}

std::string RobotCapabilities::toString() const 
{
	std::string ret;

	ret = "Team: " + std::to_string(team_) + "\n";

	for (auto pair : double_columns_)
	{
		ret += pair.first + ": ";
		ret += std::to_string(values_[pair.second]);
		ret += "\n";
	}

	for (auto pair : dist_columns_)
	{
		ret += pair.first + ": ";
		ret += dists_[pair.second].toString();
		ret += "\n";
	}

	return ret;
}
