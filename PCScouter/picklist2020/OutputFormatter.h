#pragma once

#include "Pick.h"
#include <string>
#include <list>

class RobotCapabilities;


class OutputFormatter
{
public:
	OutputFormatter() = delete;
	~OutputFormatter() = delete;

	enum class OutputType
	{
		CSV,
		HTML
	};

	static bool outputPicklist(int team, const std::string& filename, const std::vector<Pick>& picks, OutputType type);
	static bool outputRobotCapabilities(const std::string &filename, std::list<const RobotCapabilities*>& robots, OutputType type);

private:
	static bool outputPicklistCSV(int team, const std::string& filename, const std::vector<Pick>& picks);
	static bool outputPicklistHTML(int team, const std::string& filename, const std::vector<Pick>& picks);
	static bool outputRobotCapabilitiesCSV(const std::string& filename, std::list<const RobotCapabilities*>& robots);
	static bool outputRobotCapabilitiesHTML(const std::string& filename, std::list<const RobotCapabilities*>& robots);
};

