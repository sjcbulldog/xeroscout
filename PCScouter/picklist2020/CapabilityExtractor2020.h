#pragma once

#include "CapabilityExtractor.h"

class CapabilityExtractor2020 : public CapabilityExtractor
{
public:
	CapabilityExtractor2020(const Headers& headers, const DataArray& data);
	virtual ~CapabilityExtractor2020();

	bool createRobotCapabilities();

private:
	void createBallSet(const std::list<Result*>& res, std::vector<int>& set, int highcol, int lowcol);

	double wheel(int team, const std::list<AllianceGroup*>& mat, int threshold, int auto_low, int auto_high, int teleop_low, int teleop_high, int spin);
	double assisted2(int team, const std::list<AllianceGroup*>& mat, int climbed, int climb_was_assisted, int climb_assists);
	double assisted1(int team, const std::list<AllianceGroup*>& mat, int climbed, int climb_was_assisted, int climb_assists);
	double park(const std::list<Result*>& res, int climbed, int parked);
	double balance(const std::list<Result*>& res, int climbed, int climbed_was_assisted, int balance);

};

