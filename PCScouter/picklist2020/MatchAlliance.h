#pragma once

#include "Distribution.h"
#include "RobotCapabilities.h"
#include <vector>
#include <numeric>
#include <functional>
#include <algorithm>

class RobotCapabilities;

class MatchAlliance
{
public:
	MatchAlliance(std::vector<const RobotCapabilities*>&& robots);
	MatchAlliance(const std::vector<const RobotCapabilities*>& robots);
	virtual ~MatchAlliance();

	const RobotCapabilities* robot(size_t index) const {
		if (index < robots_.size())
			return robots_[index];

		return nullptr;
	}

	double sum(int col) const {
		double ret = 0;

		for (auto r : robots_)
			ret += r->doubleValue(col);

		return ret;
	}

	double max(int col) const {
		double ret = 0;

		for (auto r : robots_)
			ret = std::max(ret, r->doubleValue(col));

		return ret;
	}

	double sum(int col, std::function<double(int index, const RobotCapabilities *r)> cb) const {
		double ret = 0;

		int index = 0;
		for (auto r : robots_)
		{
			ret += (r->doubleValue(col) * cb(index++, r));
		}

		return ret;
	}

	Distribution sumDistribution(int col) const {
		Distribution dist;

		for (auto r : robots_) {
			dist = dist + r->distValue(col);
		}
		return dist;
	}

private:
	std::vector<const RobotCapabilities*> robots_;
};

