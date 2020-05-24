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
