#pragma once

#include <string>
#include <array>

enum class ClimbingStrategy
{
	None,
	Park,
	Self,
	Assisted
};

inline std::string
toString(ClimbingStrategy st)
{
	std::string ret = "????";

	switch (st)
	{
	case ClimbingStrategy::None:
		ret = "None";
		break;
	case ClimbingStrategy::Park:
		ret = "Park";
		break;

	case ClimbingStrategy::Self:
		ret = "Self";
		break;

	case ClimbingStrategy::Assisted:
		ret = "Assisted";
		break;
	}

	return ret;
}

extern const std::array<ClimbingStrategy, 4> AllClimbingStrategies;