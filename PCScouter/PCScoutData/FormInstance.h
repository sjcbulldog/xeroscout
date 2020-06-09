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

				FormItemDisplay* displayItem(const QString& name) {
					auto it = items_.find(name);
					if (it == items_.end())
						return nullptr;

					return it->second;
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
