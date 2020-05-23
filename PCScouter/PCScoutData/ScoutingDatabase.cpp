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

#include "ScoutingDatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			ScoutingDatabase ScoutingDatabase::theone_;

			ScoutingDatabase::ScoutingDatabase()
			{
				inited_ = false;
			}

			ScoutingDatabase::~ScoutingDatabase()
			{
			}

			void ScoutingDatabase::clearAllTables()
			{
				QStringList list = tables_;

				for (const QString& table : list)
					deleteTable(table);
			}

			bool ScoutingDatabase::init()
			{
				if (!inited_)
				{
					db_ = QSqlDatabase::addDatabase("QSQLITE");
					if (!db_.isValid())
						return false;

					if (!db_.open())
						return false;

					inited_ = true;
				}

				return true;
			}

			bool ScoutingDatabase::deleteTable(const QString& name)
			{
				QString qstr;
				QSqlQuery query(db_);

				qstr = "DROP TABLE " + name + ";";
				if (!query.exec(qstr))
				{
					QSqlError err = query.lastError();
					qDebug() << err.text();
					return false;
				}

				tables_.removeOne(name);
				return true;
			}

			bool ScoutingDatabase::addTable(const QString& name, const ScoutingDataSet& set)
			{
				if (set.rowCount() == 0)
					return false;

				QString qstr;
				QSqlQuery query(db_);

				qstr = "CREATE TABLE " + name + " ( ";

				auto headers = set.headers();
				auto types = set.types();

				assert(headers.size() == types.size());

				for (int i = 0; i < headers.size(); i++) {
					if (i != 0)
						qstr += ",  ";

					qstr += headers[i];
					qstr += " ";

					if (types[i] == QVariant::Type::Bool) {
						qstr += "INTEGER";
					}
					else if (types[i] == QVariant::Type::Int) {
						qstr += "INTEGER";
					}
					else if (types[i] == QVariant::Type::String) {
						qstr += "TEXT";
					}
					else {
					}
				}
				qstr += " )";

				if (!query.exec(qstr))
				{
					QSqlError err = query.lastError();
					qDebug() << err.text();
					return false;
				}

				tables_.push_back(name);

				for (int row = 0; row < set.rowCount(); row++) {
					qstr = "INSERT INTO " + name + " VALUES(";
					auto rowdata = set.row(row);
					for (int col = 0; col < set.columnCount(); col++)
					{
						if (col != 0)
							qstr += ", ";
						QVariant v = rowdata[col];
						if (types[col] == QVariant::Type::Bool) {
							if (v.isValid())
								qstr += QString::number(v.toInt());
							else
								qstr += "NULL";
						}
						else if (types[col] == QVariant::Type::Int) {
							if (v.isValid())
								qstr += QString::number(v.toInt());
							else
								qstr += "NULL";
						}
						else if (types[col] == QVariant::Type::String) {
							if (v.isValid())
								qstr += "'" + v.toString() + "'";
							else
								qstr += "NULL";
						}
						else {
							assert(false);
						}
					}
					qstr += ")";
					qDebug() << "SQL: " << qstr;

					if (!query.exec(qstr))
					{
						QSqlError err = query.lastError();
						qDebug() << err.text();
						return false;
					}
				}

				return true;
			}

			bool ScoutingDatabase::executeQuery(const QString& qstr, ScoutingDataSet& set, QString& error)
			{
				QSqlQuery query(db_);
				bool headers = false;

				set.clear();
				qDebug() << "SQL: " << qstr;
				if (!query.exec(qstr))
				{
					QSqlError err = query.lastError();
					qDebug() << err.text();
					error = err.text();

					return false;
				}

				while (query.next()) {
					auto rec = query.record();

					if (!headers) {
						for (int col = 0; col < rec.count(); col++) {
							QSqlField f = rec.field(col);
							set.addHeader(f.name(), f.type());
						}
						headers = true;
					}

					set.newRow();
					for (int col = 0; col < rec.count(); col++)
						set.addData(rec.value(col));
				}

				query.finish();

				return true;
			}
		}
	}
}
