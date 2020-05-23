//
// Copyright 2020 by Jack W. (Butch) Griffin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
