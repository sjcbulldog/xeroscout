#pragma once

#include <vector>
#include <algorithm>

class Pick
{
public:
	Pick(int team, double score) {
		team_ = team;
		nominal_score_ = score;
	}

	virtual ~Pick() {
	}

	int team() const {
		return team_;
	}

	double nominalScore() const {
		return nominal_score_;
	}

	void sort() {
		std::sort(scores_.begin(), scores_.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool { return a.second > b.second; });
	}

	void addScore(int team, double score) {
		scores_.push_back(std::make_pair(team, score));
	}

	const std::vector<std::pair<int, double>>& scores() const {
		return scores_;
	}

private:
	int team_;
	double nominal_score_;

	std::vector<std::pair<int, double>> scores_;
};

