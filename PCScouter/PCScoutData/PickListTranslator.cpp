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
