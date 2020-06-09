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

#include <QString>
#include <QVariant>
#include <map>
#include <memory>
#include <cassert>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			typedef std::map<QString, QVariant> ScoutingDataMap;
			typedef std::shared_ptr<ScoutingDataMap> ScoutingDataMapPtr;
			typedef std::shared_ptr<const ScoutingDataMap> ConstScoutingDataMapPtr;

			inline bool compareTwoMaps(ConstScoutingDataMapPtr m1, ConstScoutingDataMapPtr m2) {
				//
				// This assumes well formed property maps that are being compared.  The two maps contain exactly
				// the same properties, but the properties may have differing values
				//
				for (auto item : *m1) {
					if (item.first == "TabletName" || item.first == "TabletUUID" || item.first == "TimeStamp")
						continue;

					auto it = m2->find(item.first);
					assert(it != m2->end());

					if (it->second != item.second)
						return false;
				}

				for (auto item : *m2) {
					if (item.first == "TabletName" || item.first == "TabletUUID" || item.first == "TimeStamp")
						continue;

					auto it = m1->find(item.first);
					assert(it != m1->end());

					if (it->second != item.second)
						return false;
				}

				return true;
			}
		}
	}
}
