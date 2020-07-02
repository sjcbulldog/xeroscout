#include "MatchAlliance.h"
#include "RobotCapabilities.h"

MatchAlliance::MatchAlliance(std::vector<const RobotCapabilities*>&& robots)
{
	robots_ = std::move(robots);
}

MatchAlliance::MatchAlliance(const std::vector<const RobotCapabilities*>& robots)
{
	robots_ = robots;
}

MatchAlliance::~MatchAlliance()
{
}
