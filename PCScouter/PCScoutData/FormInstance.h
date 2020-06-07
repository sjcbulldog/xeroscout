//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "pcscoutdata_global.h"
#include "FormItemDisplay.h"
#include "ScoutingForm.h"
#include "ScoutingDataMap.h"
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT FormInstance
			{
			public:
				FormInstance(std::shared_ptr<const ScoutingForm> form);
				virtual ~FormInstance();

				std::shared_ptr<const ScoutingForm> form() {
					return form_;
				}

				void extract(ScoutingDataMapPtr data);
				void assign(ConstScoutingDataMapPtr data);

				void addDisplayItem(const QString& tag, FormItemDisplay* item) {
					items_.insert_or_assign(tag, item);
				}

				void clear() {
					for (auto pair : items_)
					{
						delete pair.second;
					}
					items_.clear();
				}

			private:
				std::shared_ptr<const ScoutingForm> form_;
				std::map<QString, FormItemDisplay*> items_;
			};
		}
	}
}
