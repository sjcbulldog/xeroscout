#include "Result.h"
#include "Data.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

Result::Result(const Headers& headers, const DataRow& row) : headers_(headers)
{
	size_t colno;
	
	colno = indexOf(headers, TeamColName);
	team_ = static_cast<int>(std::get<double>(row[colno]) + 0.5);

	colno = indexOf(headers, MatchColName);
	match_ = static_cast<int>(std::get<double>(row[colno]) + 0.5);

	colno = indexOf(headers, AllianceColName);
	std::string alliance = std::get<std::string>(row[colno]);
	std::transform(alliance.begin(), alliance.end(), alliance.begin(),
		[](unsigned char c) { return std::tolower(c); });
	if (alliance == "red")
		a_ = Alliance::Red;
	else if (alliance == "blue")
		a_ = Alliance::Blue;
	else
	{
		std::runtime_error err("alliance value was not 'red' or 'blue'");
		throw err;
	}

	for (size_t i = 0; i < headers.size(); i++)
	{
		double v;

		if (headers[i] == AllianceColName)
			v = 0.0;
		else
			v = std::get<double>(row[i]);

		values_.push_back(v);
	}
}

Result::~Result()
{
}