#pragma once

#include <vector>
#include <string>
#include <variant>
#include <algorithm>

typedef std::variant<double, std::string> DataElem;
typedef std::vector<DataElem> DataRow;
typedef std::vector<std::string> Headers;
typedef std::vector<DataRow> DataArray;

inline size_t indexOf(const Headers& headers, const std::string& name) {
	auto it = std::find(headers.begin(), headers.end(), name);
	if (it == headers.end())
		return std::numeric_limits<size_t>::max();

	auto dist = std::distance(headers.begin(), it);
	return dist;
}
