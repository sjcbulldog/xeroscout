#pragma once

#include "Data.h"
#include "Result.h"
#include "AllianceGroup.h"
#include "RobotCapabilities.h"
#include <string>
#include <variant>
#include <vector>
#include <list>
#include <set>
#include <functional>

class TeamPicker
{
public:
	TeamPicker(const Headers& headers);
	virtual ~TeamPicker();

	virtual bool createPickList(int team) = 0;

protected:
	virtual bool isDataOk();
	bool checkColumns(const std::list<std::string>& columns);

private:
	const Headers& headers_;
	static std::list<std::string> required_columns_;
};

