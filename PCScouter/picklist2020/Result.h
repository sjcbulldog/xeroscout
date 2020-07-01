#pragma once

#include "Alliance.h"
#include "Data.h"
#include <map>
#include <string>
#include <algorithm>
#include <cassert>

class Result
{
public:
	Result(const Headers& headers, const DataRow& row);
	virtual ~Result();

	int team() const {
		return team_;
	}

	int match() const {
		return match_;
	}

	Alliance alliance() const {
		return a_;
	}

	double value(int column) const {
		return values_[column];
	}

private:
	static constexpr const char* TeamColName = "team";
	static constexpr const char* MatchColName = "match";
	static constexpr const char* AllianceColName = "alliance";

private:
	int team_;
	int match_;
	Alliance a_;

	//
	// The values are stored in the same order as the headers.  The
	// the values above are broken out for easy access.  Since the value_ array
	// cannot store the alliance, it is just stored as zero in the array below
	// as the above Alliance a_ value should be used to access the alliance.
	//
	std::vector<double> values_;
	const Headers& headers_;
};

