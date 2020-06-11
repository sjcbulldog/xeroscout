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

#pragma once

#include <string>
#include <list>

/// \file

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			/// \brief This class contains static methods for converting values from one set of units to another
			class UnitConverter
			{
			public:
				UnitConverter() = delete;
				~UnitConverter() = delete;

				/// \brief convert a double value from one set of units to another
				/// \param value the value to convert
				/// \param from the units to convert from
				/// \param to the units to convert to
				/// \returns value converted to new units
				static double convert(double value, const std::string& from, const std::string& to);

				/// \brief convert a double value from one set of units to another
				/// \param value the value to convert
				/// \param from the units to convert from
				/// \param to the units to convert to
				/// \return value converted to new units
				static float convert(float value, const std::string& from, const std::string& to);

				/// \brief returns information about whether a conversion is possible
				/// \param from the units to convert from
				/// \param to the units to convert to
				/// \returns true if the conversion is possible, false otherwise
				static bool hasConversion(const std::string& from, const std::string& to);

				/// \brief returns all units the converter can process
				/// \returns a list of all units the converter can process
				static std::list<std::string> getAllUnits();

			private:
				struct conversion
				{
					const char* from;
					const char* to;
					double conversion;
				};

				static bool findConversion(const std::string& from, const std::string& to, double& conversion);
				static struct conversion convert_[];
			};
		}
	}
}