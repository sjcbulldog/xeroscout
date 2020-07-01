#pragma once

#include "Data.h"
#include "Result.h"
#include "AllianceGroup.h"
#include "RobotCapabilities.h"
#include <functional>
#include <list>
#include <cassert>
#include <cmath>

class CapabilityExtractor
{
public:
	CapabilityExtractor(const Headers& headers, const DataArray& data);
	virtual ~CapabilityExtractor();

	RobotCapabilities* robot(int team) {
		auto it = robots_.find(team);
		if (it == robots_.end())
			return nullptr;

		return it->second;
	}

	std::list<int> teams() {
		std::list<int> result;

		for (auto pair : team_matches_)
			result.push_back(pair.first);

		return result;
	}

	std::list<AllianceGroup*> matches(int team) {
		std::list<AllianceGroup*> list;

		auto it = team_matches_.find(team);
		if (it != team_matches_.end())
			list = it->second;

		return list;
	}

	std::list<Result*> results(int team) {
		std::list<Result*> list;

		auto it = team_results_.find(team);
		if (it != team_results_.end())
			list = it->second;

		return list;
	}

protected:

	//
	// We store everything as a double.  Therefore, to check boolean values, we test to be sure
	// they are larger than some really small value
	//
	static bool testBoolean(const Result* r, int col) {
		double d = r->value(col);
		return std::fabs(d) > 0.001;
	}

	//
	// Iterate across a set of boolean columns and perform a logical OR
	//
	static double logicalOR(Result* res, const std::vector<int>& cols, const std::vector<bool>& invert = std::vector<bool>()) {
		for (size_t i = 0; i < cols.size(); i++)
		{
			int col = cols[i];
			bool inv = false;
			if (i < invert.size())
				inv = invert[i];

			bool v = testBoolean(res, col);
			if (inv)
				v = !v;

			if (v)
				return 1.0;
		}

		return 0.0;
	}

	//
	// Iterate across a set of boolean columns and perform a logical OR
	//
	static double logicalAND(Result* res, const std::vector<int>& cols, const std::vector<bool>& invert = std::vector<bool>()) {
		for (size_t i = 0; i < cols.size(); i++)
		{
			int col = cols[i];
			bool inv = false;
			if (i < invert.size())
				inv = invert[i];

			bool v = testBoolean(res, col);
			if (inv)
				v = !v;

			if (!v)
				return 0.0;
		}

		return 1.0;
	}

	void averageResultsValue(RobotCapabilities* robot, const std::list<Result*> res, const std::string& colname) {
		int col = column(colname);
		assert(col != -1);
		robot->addDouble(colname, average(res, col));
	}

	std::list<RobotCapabilities*> robots() const {
		std::list<RobotCapabilities*> list;

		for (auto pair : robots_)
		{
			list.push_back(pair.second);
		}

		return list;
	}

	static double sum(const std::list<Result*>& list, int col) {
		double ret = 0.0;
		for (auto r : list) {
			ret += r->value(col);
		}
		return ret;
	}

	static double average(const std::list<Result*>& list, int col) {
		assert(list.size() != 0);
		return sum(list, col) / static_cast<double>(list.size());
	}

	static double sum(const std::list<Result*>& list, std::function<double(Result*)> func) {
		double ret = 0.0;
		for (auto r : list) {
			ret += func(r);
		}
		return ret;
	}

	static double average(const std::list<Result*>& list, std::function<double(Result*)> func) {
		assert(list.size() != 0);
		return sum(list, func) / static_cast<double>(list.size());
	}

	bool hasValue(const std::string& name) const {
		auto it = std::find(headers_.begin(), headers_.end(), name);
		return it != headers_.end();
	}

	int column(const std::string& name) const {
		auto it = std::find(headers_.begin(), headers_.end(), name);
		assert(it != headers_.end());
		return static_cast<int>(std::distance(headers_.begin(), it));
	}

	void addRobot(int team, RobotCapabilities* robot) {
		robots_.insert_or_assign(team, robot);
	}

	void addMatchToTeam(int team, AllianceGroup* match);
	void addResultToTeam(int team, Result* result);

	bool createResults();
	bool createMatches();

private:
	const Headers& headers_;
	std::map<int, RobotCapabilities*> robots_;

	const DataArray& data_;

	std::vector<Result*> results_;
	std::vector<AllianceGroup*> matches_;

	std::map<int, std::list<Result*>> team_results_;
	std::map<int, std::list<AllianceGroup*>> team_matches_;
};

