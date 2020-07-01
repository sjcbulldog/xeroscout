#pragma once

#include "Alliance.h"
#include "Result.h"
#include <vector>
#include <cassert>

class Result;

class AllianceGroup
{
public:
	AllianceGroup(int match, Alliance a);
	virtual ~AllianceGroup();

	size_t size() {
		return results_.size();
	}

	void addResult(Result* r) {
		results_.push_back(r);
	}

	const Result* result(size_t index) const {
		return results_[index];
	}

	Result* result(size_t index) {
		return results_[index];
	}

	double sum(int col) const {
		return results_[0]->value(col) + results_[1]->value(col) + results_[2]->value(col);
	}

	const Result* teamResult(int team) const {
		assert(results_.size() == 3);
		
		//
		// Not sure if this is faster than a loop, but it cant really be slower
		//
		if (results_[0]->team() == team)
			return results_[0];

		if (results_[1]->team() == team)
			return results_[1];

		if (results_[2]->team() == team)
			return results_[2];

		return nullptr;
	}

private:
	int match_;
	Alliance a_;
	std::vector<Result*> results_;
};

