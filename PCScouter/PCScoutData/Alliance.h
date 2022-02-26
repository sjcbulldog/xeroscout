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

#include <QtCore/QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			enum class Alliance {
				Red,
				Blue,
				Both,
				Invalid
			};

			inline QString toString(Alliance c) {
				QString ret;

				if (c == Alliance::Red)
					ret = "red";
				else if (c == Alliance::Blue)
					ret = "blue";
				else if (c == Alliance::Both)
					ret = "both";
				else
				{
					assert(false);
					ret = "both";
				}

				return ret;
			}

			inline Alliance allianceFromString(const QString& str) {
				Alliance ret = Alliance::Invalid;

				if (str == "red")
					ret = Alliance::Red;
				else if (str == "blue")
					ret = Alliance::Blue;
				else if (str == "both")
					ret = Alliance::Both;
				
				return ret;
			}
		}
	}
}
