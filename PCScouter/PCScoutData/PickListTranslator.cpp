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

#include "PickListTranslator.h"
#include "QJsonObject"
#include <QFile>
#include <QJsonParseError>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			PickListTranslator::PickListTranslator()
			{
			}

			PickListTranslator::~PickListTranslator()
			{
			}

			bool PickListTranslator::load(const QString& filename)
			{
				QFile file(filename);
				if (!file.open(QIODevice::ReadOnly))
				{
					error_ = "cannot open file '" + filename + "' for reading";
					return false;
				}

				QByteArray data = file.readAll();
				QJsonParseError err;
				QJsonDocument doc;
				doc = QJsonDocument::fromJson(data, &err);

				if (doc.isNull())
				{
					error_ = err.errorString();
					return false;
				}

				if (!doc.isArray())
				{
					error_ = "expected JSON array in JSON document";
					return false;
				}

				return load(doc.array());
			}

			bool PickListTranslator::load(const QJsonArray& array)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
					{
						error_ = "entry " + QString::number(i + 1) + " is not a JSON object";
						return false;
					}

					QJsonObject obj = array[i].toObject();
					if (obj.count() != 2)
					{
						error_ = "entry " + QString::number(i + 1) + " expected two field, 'output' and 'data', got " + QString::number(obj.count()) + " field";
						return false;
					}

					if (!obj.contains("output") || !obj.value("output").isString())
					{
						error_ = "entry " + QString::number(i + 1) + " missing field 'output' with a string value";
						return false;
					}

					if (!obj.contains("data") || !obj.value("data").isString())
					{
						error_ = "entry " + QString::number(i + 1) + " missing field 'data' with a string value";
						return false;
					}

					fields_.push_back(std::make_pair(obj.value("output").toString(), obj.value("data").toString()));
				}

				return true;
			}

			QJsonArray PickListTranslator::toJSON() const
			{
				QJsonArray ret;

				for (auto pair : fields_)
				{
					QJsonObject obj;

					obj["output"] = pair.first;
					obj["data"] = pair.second;
					ret.push_back(obj);
				}
				return ret;
			}
		}
	}
}
