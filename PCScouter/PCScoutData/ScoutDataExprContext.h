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


