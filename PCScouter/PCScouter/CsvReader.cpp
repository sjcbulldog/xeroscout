#include "CsvReader.h"
#include <fstream>
#include <iostream>
#include <cassert>

CsvReader::CsvReader(bool same)
{
	same_ = same;
	rownumb_ = 0;
}

CsvReader::~CsvReader()
{
}

bool CsvReader::readFile(const std::filesystem::path& path, HeaderType hdrs)
{
	DataRow row;

	if (!std::filesystem::exists(path))
	{
		std::cerr << "CsvReader: file '" << path.generic_string() << "' does not exist" << std::endl;
		return false;
	}

	if (!std::filesystem::is_regular_file(path))
	{
		std::cerr << "CsvReader: file '" << path.generic_string() << "' exists but is not a file" << std::endl;
		return false;
	}

	std::ifstream in(path);
	if (!in.is_open())
	{
		std::cerr << "CsvReader: file '" << path.generic_string() << "' cannot be opened for reading" << std::endl;
		return false;
	}

	if (hdrs == HeaderType::Headers)
	{
		if (!readHeaders(in))
			return false;

	}
	else if (hdrs == HeaderType::AutoDetect)
	{
		if (!readRow(in, row))
			return false;

		bool allstr = true;
		for (int i = 0; i < row.size(); i++)
		{
			if (!std::holds_alternative<std::string>(row[i]))
			{
				allstr = false;
				break;
			}
		}

		if (allstr)
		{
			//
			// The first row is all strings, assume it is headers
			//
			for (int i = 0; i < row.size(); i++)
			{
				headers_.push_back(std::get<std::string>(row[i]));
			}
		}
		else
		{
			data_.push_back(row);
		}	
	}

	if (!readData(in))
		return false;

	return true;
}

bool CsvReader::readHeaders(std::istream& in)
{
	DataRow row;
	if (!readRow(in, row, true))
		return false;


	for (int i = 0; i < row.size(); i++)
	{
		if (!std::holds_alternative<std::string>(row[i]))
		{
			std::cerr << "CSVReader: requst to read headers, but first row was not all strings" << std::endl;
			return false;
		}
		headers_.push_back(std::get<std::string>(row[i]));
	}

	return true;
}

bool CsvReader::readData(std::istream& in)
{
	DataRow row;
	bool ret = true;

	while (!in.eof())
	{
		if (!readRow(in, row))
			return false;

		if (row.size() == 0)
			continue;

		if (same_)
		{
			if (headers_.size() > 0)
			{
				//
				// If we have headers, each row must match the headers
				//
				if (row.size() != headers_.size())
				{
					std::cerr << "CsvReader: row " << rownumb_ << ": number of elements, " << row.size();
					std::cerr << " does not match the number of headers " << headers_.size() << std::endl;
					ret = false;
				}
				data_.push_back(row);
			}
			else if (data_.size() > 0)
			{
				if (row.size() != data_[0].size())
				{
					std::cerr << "CsvReader: row " << rownumb_ << ": number of elements, " << row.size();
					std::cerr << " does not match the number of elements in the first row " << data_[0].size() << std::endl;
					ret = false;
				}
				data_.push_back(row);
			}
			else
			{
				data_.push_back(row);
			}
		}
		else
		{
			data_.push_back(row);
		}

	}

	return ret;
}

bool CsvReader::skipSpaces(std::istream& in)
{
	while (1)
	{
		if (in.eof())
			return true;

		if (in.fail() || in.bad())
			return false;

		int ch = in.peek();
		if (ch != ' ' && ch != '\t' && ch != '\r')
			break;

		in.get();
	}

	return true;
}

bool CsvReader::readToken(std::istream& in, std::string& token)
{
	int ch;

	while (true)
	{
		if (in.bad() || in.fail())
			return false;

		ch = in.peek();
		if (ch == EOF)
			return true;

		if (ch == ',' || ch == '\n')
			break;

		ch = in.get();
		token += ch;
	}

	return true;
}

bool CsvReader::readQuoted(std::istream& in, std::string& token)
{
	int ch;

	ch = in.get();
	assert(ch == '"');

	while (true)
	{
		if (in.bad() || in.fail())
			return false;

		ch = in.get();
		if (ch == EOF)
		{
			std::cerr << "CsvReader: row " << rownumb_ << ": end of file encountered while reading string" << std::endl;
			return false;
		}

		if (ch == '\n')
		{
			//
			// Some CSV readers all new lines in quoted string, but not this one
			//
			std::cerr << "CsvReader: row " << rownumb_ << ": end of line encountered while reading string" << std::endl;
			return false;
		}

		if (ch == '"')
		{
			if (in.peek() == '"')
			{
				in.get();
				token += ch;
			}
			else
			{
				break;
			}
		}
		else
		{
			token += ch;
		}
	}

	return true;
}

bool CsvReader::readRow(std::istream& in, DataRow& row, bool forcestring)
{
	bool looping = true;
	bool instr = false;

	row.clear();

	rownumb_++;
	while (looping)
	{
		// if the file is at EOF, this function returns true but an empty row
		if (in.eof())
			return true;

		if (in.bad() || in.fail())
			return false;

		//
		// Try to read a row of data
		//
		while (true)
		{
			std::string token;

			if (!skipSpaces(in))
				return false;

			int ch = in.peek();
			if (ch == EOF)
			{
				looping = false;
				break;
			}

			if (ch == '"')
			{
				if (!readQuoted(in, token))
					return false;

				//
				// Quoted string are always string
				//
				row.push_back(DataElem(token));
			}
			else
			{
				if (!readToken(in, token))
					return false;

				//
				// Need to deduce what this is, basically if it parses as floating point
				// number, its a double, otherwise a string
				//
				bool parsed = false;
				size_t loc;
				if (!forcestring)
				{
					try {
						double v = std::stod(token, &loc);
						if (loc == token.length())
						{
							parsed = true;
							row.push_back(DataElem(v));
						}
					}
					catch (const std::invalid_argument&) {
					}
				}

				if (!parsed)
					row.push_back(DataElem(token));
			}

			if (!skipSpaces(in))
				return false;

			ch = in.get();
			if (ch == '\n' || ch == -1)
			{
				break;
			}
			else if (ch == ',')
			{
			}
			else
			{
				std::cerr << "CsvReader: row " << rownumb_ << ": invalid text in row" << std::endl;
				return false;
			}
		}

		if (row.size() > 0)
			break;
	}

	return true;
}