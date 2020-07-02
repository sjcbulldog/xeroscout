#pragma once

#include "Distribution.h"
#include <map>
#include <string>
#include <vector>

class RobotCapabilities
{
public:
	RobotCapabilities(int team);
	virtual ~RobotCapabilities();

	int team() const {
		return team_;
	}

	static std::vector<std::string> doubleColumnNames() {
		std::vector<std::string> ret;

		for (auto pair : double_columns_)
		{
			ret.push_back(pair.first);
		}

		return ret;
	}

	static std::vector<std::string> distColumnNames() {
		std::vector<std::string> ret;

		for (auto pair : dist_columns_)
		{
			ret.push_back(pair.first);
		}

		return ret;
	}

	void addDouble(const std::string& name, double value);
	void addDistribution(const std::string& name, const Distribution& dist);

	static int doubleColumn(const std::string& name) {
		auto it = double_columns_.find(name);
		if (it == double_columns_.end())
			return -1;

		return it->second;
	}

	static int distColumn(const std::string& name) {
		auto it = dist_columns_.find(name);
		if (it == dist_columns_.end())
			return -1;

		return it->second;
	}

	double doubleValue(int column) const {
		return values_[column];
	}

	const Distribution& distValue(int column) const {
		return dists_[column];
	}

	std::string toString() const;

	template<typename Iterator>
	static RobotCapabilities mean(int team, Iterator start, Iterator end) {
		RobotCapabilities cap(team);

		for (auto pair : double_columns_) {
			int col = doubleColumn(pair.first);
			double total = 0.0;
			int count = 0;

			for (Iterator it = start; it != end; it++)
			{
				const RobotCapabilities* r = *it;
				total += r->doubleValue(col);
				count++;
			}

			cap.addDouble(pair.first, total / count);
		}

		for (auto pair : dist_columns_) {
			int col = distColumn(pair.first);

			std::vector<Distribution> dists;
			for (Iterator it = start; it != end; it++)
			{
				const RobotCapabilities* r = *it;
				dists.push_back(r->distValue(col));
			}

			cap.addDistribution(pair.first, Distribution::mean(dists));
		}

		return cap;
	}

private:
	int team_;
	std::vector<double> values_;
	std::vector<Distribution> dists_;

	static std::map<std::string, int> double_columns_;
	static std::map<std::string, int> dist_columns_;
};

