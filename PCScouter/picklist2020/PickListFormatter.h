#pragma once

#include "Pick.h"
#include <string>

class PickListFormatter
{
public:
	PickListFormatter() = delete;
	~PickListFormatter() = delete;

	enum class OutputType
	{
		CSV,
		HTML
	};

	static bool outputData(int team, const std::string& filename, const std::vector<Pick>& picks, OutputType type);

private:
	static bool outputDataCSV(int team, const std::string& filename, const std::vector<Pick>& picks);
	static bool outputDataHTML(int team, const std::string& filename, const std::vector<Pick>& picks);
};

