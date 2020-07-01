#include "MatchAlliance.h"
#include "RobotCapabilities.h"

MatchAlliance::MatchAlliance(std::vector<RobotCapabilities*>&& robots)
{
	robots_ = std::move(robots);
}

MatchAlliance::MatchAlliance(const std::vector<RobotCapabilities*>& robots)
{
	robots_ = robots;
}

MatchAlliance::~MatchAlliance()
{
}
