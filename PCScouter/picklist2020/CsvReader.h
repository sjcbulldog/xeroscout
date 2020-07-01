#pragma once

#include "Data.h"
#include <filesystem>
#include <string>
#include <vector>
#include <variant>

class CsvReader
{
public:
	enum class HeaderType
	{
		Headers,
		AutoDetect,
		NoHeaders
	};

public:
	CsvReader(bool same);
	virtual ~CsvReader();

	bool readFile(const std::filesystem::path& path, HeaderType hdrs);

	size_t rowCount() const {
		return data_.size();
	}

	size_t colCount() const {
		if (data_.size() == 0)
			return 0;

		return (data_[0]).size();
	}

	const DataElem& get(size_t row, size_t col) const {
		return (data_[row])[col];
	}

	const DataRow& getRow(size_t row) {
		return data_[row];
	}

	const Headers& headers() const {
		return headers_;
	}

	const DataArray& data() const {
		return data_;
	}


private:
	bool readHeaders(std::istream& in);
	bool readRow(std::istream& in, DataRow& row);
	bool readData(std::istream& in);
	bool readQuoted(std::istream& in, std::string& token);
	bool readToken(std::istream& in, std::string& token);
	bool skipSpaces(std::istream &in);

private:
	Headers headers_;
	DataArray data_;
	bool same_;
	int rownumb_;
};
