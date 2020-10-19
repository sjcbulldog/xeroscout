#pragma once

#include "PickListEntry.h"
#include "RobotCapabilities.h"
#include <QString>
#include <vector>

class PicklistHTMLGenerator
{
public:
	PicklistHTMLGenerator() = delete;
	~PicklistHTMLGenerator() = delete;

	static QString genPicklistHTML(int team, const std::vector<xero::scouting::datamodel::PickListEntry>& picklist, int rows, int cols);
	static QString genRobotCapabilitiesHTML(const std::vector<xero::scouting::datamodel::RobotCapabilities>& robots);
};


