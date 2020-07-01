#include "TeamPicker.h"
#include <iostream>
#include <set>

std::list<std::string> TeamPicker::required_columns_ =
{
	"team",
	"match",
	"alliance"
};

TeamPicker::TeamPicker(const Headers& headers) : headers_(headers)
{
}

TeamPicker::~TeamPicker()
{
}

bool TeamPicker::isDataOk()
{
	return checkColumns(required_columns_);
}

bool TeamPicker::checkColumns(const std::list<std::string> &columns)
{
	bool ret = true;

	for (const std::string& column : columns)
	{
		if (indexOf(headers_, column) >= headers_.size())
		{
			std::cerr << "TeamPicker: required column '" << column << "' was not found in the data" << std::endl;
			ret = false;
		}
	}

	return ret;
}
