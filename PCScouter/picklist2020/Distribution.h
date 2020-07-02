#pragma once

#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>

class Distribution
{
	friend Distribution operator+(const Distribution&, const Distribution&);

public:
	Distribution() {
		dist_[0] = 1.0;
	}

	template <typename Iterator>
	Distribution(Iterator start, Iterator end) {
		std::map<int, int> counts;
		double total = 0.0;

		for(Iterator sit = start ; sit != end ; sit++) {
			total += 1.0;

			int value = *sit;
			auto it = counts.find(value);
			if (it == counts.end())
			{
				counts.insert_or_assign(value, 1);
			}
			else 
			{
				counts[value]++;
			}
		}

		for (auto pair : counts)
		{
			double pcnt = static_cast<double>(pair.second) / total;
			dist_.insert_or_assign(pair.first, pcnt);
		}
	}

	virtual ~Distribution() 
	{
	}

	const std::map<int, double>& data() const {
		return dist_;
	}

	void clear() {
		dist_.clear();
	}

	void assertValid()
	{
		double total = 0.0;

		for (auto pair : dist_)
		{
			total += pair.second;
		}

		assert(std::fabs(total - 1.0) < 0.1);
	}

	Distribution capAt(int n) const {
		Distribution ret;

		ret.clear();
		for (auto pair : dist_)
		{
			if (pair.first > n)
			{
				ret.add(n, pair.second);
			}
			else
			{
				ret.add(pair.first, pair.second);
			}
		}

		return ret;
	}

	void add(int value, double chance) {
		dist_[value] += chance;
	}

	std::string toString() const {
		std::string ret;

		for (auto pair : dist_)
		{
			ret += "[ " + std::to_string(pair.first) + ", " + std::to_string(pair.second) + " ]";
		}

		return ret;
	}

	static Distribution mean(const std::vector<Distribution> &array) {
		Distribution ret;

		ret.clear();
		if (array.size()) {
			for (const Distribution& dist : array)
			{
				for (auto pair : dist.dist_)
				{
					ret.add(pair.first, pair.second / array.size());
				}
			}
		}

		return ret;
	}

	double chances(int value) const {
		double ret = 0.0;

		for (auto pair : dist_) {
			if (pair.first >= value)
				ret += pair.second;
		}

		return ret;
	}

private:
	std::map<int, double> dist_;
};

inline 
Distribution operator+(const Distribution& a, const Distribution& b) 
{
	Distribution ret;

	ret.clear();
	for (auto p1 : a.dist_)
	{
		for (auto p2 : b.dist_)
		{
			ret.add(p1.first + p2.first, p1.second * p2.second);
		}
	}

	return ret;
}
