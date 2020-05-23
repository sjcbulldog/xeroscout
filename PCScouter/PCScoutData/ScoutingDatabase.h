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

#include "ScoutingDataSet.h"
#include <QSqlDatabase>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ScoutingDatabase
			{
			public:
				ScoutingDatabase();
				virtual ~ScoutingDatabase();

				static ScoutingDatabase* getInstance() {
					if (!theone_.init())
						return nullptr ;

					return &theone_;
				}

				bool init();
				bool deleteTable(const QString& name);
				bool addTable(const QString& name, const ScoutingDataSet& set);
				bool executeQuery(const QString& query, ScoutingDataSet& set, QString& error);
				void clearAllTables();

				bool hasTable(const QString& table) {
					return tables_.contains(table);
				}

			private:
				static ScoutingDatabase theone_;

			private:
				bool inited_;
				QSqlDatabase db_;

				QStringList tables_;
			};

		}
	}
}
