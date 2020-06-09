//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#include "UnitConverter.h"
#include <stdexcept>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		UnitConverter::conversion UnitConverter::convert_[] =
		{
			{ "ft", "in", 12.0},
			{ "feet", "in", 12.0},
			{ "foot", "in", 12.0},
			{ "ft", "inches", 12.0},
			{ "feet", "inches", 12.0},
			{ "foot", "inches", 12.0},
			{ "in", "cm", 2.54},
			{ "in", "m", 0.0254},
			{ "in", "meters", 0.0254},
		};

		bool UnitConverter::findConversion(const std::string& from, const std::string& to, double& conversion)
		{
			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (from == convert_[i].from && to == convert_[i].to)
				{
					conversion = convert_[i].conversion;
					return true;
				}

				if (to == convert_[i].from && from == convert_[i].to)
				{
					conversion = 1.0 / convert_[i].conversion;
					return true;
				}
			}

			return false;
		}

		double UnitConverter::convert(double value, const std::string& from, const std::string& to)
		{
			double conv;

			if (from == to)
				return value;

			if (!findConversion(from, to, conv))
			{
				std::string msg = "no conversion from '";
				msg += from;
				msg += "' to '";
				msg += to;
				msg += "'";
				throw std::runtime_error(msg.c_str());
			}

			return value * conv;
		}

		float UnitConverter::convert(float value, const std::string& from, const std::string& to)
		{
			double conv;

			if (!findConversion(from, to, conv))
			{
				std::string msg = "no conversion from '";
				msg += from;
				msg += "' to '";
				msg += to;
				msg += "'";
				throw std::runtime_error(msg.c_str());
			}

			return static_cast<float>(value * conv);
		}

		bool UnitConverter::hasConversion(const std::string& from, const std::string& to)
		{
			double conv;
			return findConversion(from, to, conv);
		}

		std::list<std::string> UnitConverter::getAllUnits()
		{
			std::list<std::string> result;

			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (std::find(result.begin(), result.end(), convert_[i].to) == result.end())
					result.push_back(convert_[i].to);
				if (std::find(result.begin(), result.end(), convert_[i].from) == result.end())
					result.push_back(convert_[i].from);
			}

			return result;
		}
	}
}
