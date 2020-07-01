#pragma once

#include <string>

enum class Alliance
{
	Red,
	Blue
};

inline std::string toString(Alliance a)
{
	std::string ret;

	if (a == Alliance::Red)
		ret = "red";
	else if (a == Alliance::Blue)
		ret = "blue";
	else
		ret = "UNKNOWN";

	return ret;
}
