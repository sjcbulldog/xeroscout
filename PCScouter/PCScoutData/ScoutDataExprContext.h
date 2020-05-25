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
				ScoutDataExprContext(const QStringList &names) {
					vars_ = names;
				}

				void addValues(ScoutingDataMapPtr dm) {
					dm_ = dm;
				}

				virtual bool isValidVariable(const QString& name) {
					return vars_.contains(name);
				}

				virtual QVariant getVariable(const QString& name) {
					QVariant ret;

					if (dm_ != nullptr)
					{
						auto it = dm_->find(name);
						if (it != dm_->end())
							ret = it->second;
					}

					return ret;
				}

			private:
				QStringList vars_;
				ScoutingDataMapPtr dm_;
			};
		}
	}
}


