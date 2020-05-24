#pragma once

#include "ExprContext.h"
#include "ScoutingDataMap.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ScoutDataExprContext : public xero::expr::ExprContext
			{
			public:
				ScoutDataExprContext(ScoutingDataMapPtr dm) {
					dm_ = dm;
				}

				virtual bool isValidVariable(const QString& name) {
					auto it = dm_->find(name);
					return it != dm_->end();
				}

				virtual QVariant getVariable(const QString& name) {
					QVariant ret;

					auto it = dm_->find(name);
					if (it != dm_->end())
						ret = it->second;

					return ret;
				}

			private:
				ScoutingDataMapPtr dm_;
			};
		}
	}
}


