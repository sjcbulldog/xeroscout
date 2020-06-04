#include "InputData.h"
#include <fstream>
#include <iostream>
#include <cassert>

namespace xero
{
	namespace analysis2020
	{
		InputData::InputData()
		{
		}

		InputData::~InputData()
		{
		}

		std::vector<std::string> InputData::split(const std::string& line)
		{
			std::vector<std::string> tokens;
			size_t index = 0;
			std::string token;

			while (index < line.length())
			{
				token.clear();

				while (index < line.length() && line[index] != ',')
					token += line[index++];

				if (index == line.length())
					break;

				tokens.push_back(token);

				assert(line[index] == ',');
				index++;
			}

			if (token.length() > 0)
				tokens.push_back(token);

			return tokens;
		}

		bool InputData::readCSV(const std::string& file, std::string &error)
		{
			int lineno = 0;
			std::string line;

			std::ifstream in(file);
			if (!in.is_open())
				return false;

			while (std::getline(in, line))
			{
				row datarow;
				lineno++;

				if (lineno == 1)
				{
					columns_ = split(line);
				}
				else
				{
					size_t idx;

					auto tokens = split(line);
					if (tokens.size() != columns_.size())
						return false;

					for (const std::string& value : tokens)
					{
						double v = 0.0;

						if (value == "red" || value == "RED")
							v = 0.0;
						else if (value == "blue" || value == "BLUE")
							v = 1.0;
						else
						{
							try {
								v = std::stod(value, &idx);
								if (idx != value.length())
								{
									error = "invalid value in line " + std::to_string(lineno);
									return false;
								}
							}
							catch (...)
							{
								error = "invalid value in line " + std::to_string(lineno);
								return false;
							}
						}

						datarow.push_back(v);
					}
					data_.push_back(datarow);
				}
			}

			return true;
		}
	}
}
