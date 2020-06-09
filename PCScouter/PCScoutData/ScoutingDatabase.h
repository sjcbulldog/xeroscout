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

#include "ScoutingDataSet.h"
#include <QSqlDatabase>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ScoutingDataModel;

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
				bool executeQuery(const ScoutingDataModel& dm, const QString& query, ScoutingDataSet& set, QString& error);
				void clearAllTables();

				bool hasTable(const QString& table) {
					return tables_.contains(table);
				}

			private:
				static QString quoteString(const QString &str);

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
