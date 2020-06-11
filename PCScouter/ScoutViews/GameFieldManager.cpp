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

#include "GameFieldManager.h"
#include "GameField.h"
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			GameFieldManager::GameFieldManager()
			{
			}

			GameFieldManager::~GameFieldManager()
			{
			}

			bool GameFieldManager::hasFile(QString filename)
			{
				QString path = QString(getDefaultDir().c_str()) + "/" + filename;
				return QFile::exists(path);
			}

			void GameFieldManager::convert(const std::string& units)
			{
				for (auto field : fields_)
					field->convert(units);
			}

			bool GameFieldManager::exists(const std::string& name)
			{
				for (auto field : fields_)
				{
					if (field->getName() == name)
						return true;
				}

				return false;
			}

			std::shared_ptr<GameField>  GameFieldManager::getDefaultField()
			{
				if (fields_.size() == 0)
					return nullptr;

				return fields_.front();
			}

			std::shared_ptr<GameField> GameFieldManager::getFieldByName(const std::string& name)
			{
				for (auto field : fields_)
				{
					if (field->getName() == name)
						return field;
				}

				return nullptr;
			}

			std::list<std::string> GameFieldManager::getNames()
			{
				std::list<std::string> names;

				for (auto field : fields_)
					names.push_back(field->getName());

				return names;
			}

			std::list<std::shared_ptr<GameField>> GameFieldManager::getFields()
			{
				std::list<std::shared_ptr<GameField>> list;
				for (auto field : fields_)
					list.push_back(field);

				return list;
			}

			bool GameFieldManager::processJSONFile(QFile& file)
			{
				std::string title_value;
				std::string image_value;
				std::string units_value;
				QPointF topleft;
				QPointF bottomright;
				QPointF size;
				QString text;


				if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					qWarning() << "Cannot open file '" << file.fileName() << "' for reading";
					return false;
				}

				text = file.readAll();
				QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
				if (doc.isNull())
				{
					qWarning() << "Cannot parse file '" << file.fileName() << "' for reading";
					return false;
				}

				if (!doc.isObject())
				{
					qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
					return false;
				}

				if (!getJSONStringValue(file, doc, gameTag, title_value))
					return false;

				if (!getJSONStringValue(file, doc, imageTag, image_value))
					return false;

				if (!getJSONStringValue(file, doc, unitsTag, units_value))
					return false;

				QJsonValue v = doc[cornersTag];
				if (v.isUndefined())
				{
					qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << cornersTag
						<< "' at the top level";
					return false;
				}

				if (!v.isObject())
				{
					qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << cornersTag
						<< "' at the top level, but it is not a JSON object";
					return false;
				}

				QJsonObject obj = v.toObject();

				QJsonValue tl = obj[topLeftCornerTag];
				if (tl.isUndefined())
				{
					qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << topLeftCornerTag
						<< "' in the '" << cornersTag << "' object";
					return false;
				}
				if (!getJSONPointValue(file, tl, topLeftCornerTag, topleft))
					return false;


				QJsonValue br = obj[bottomRightCornerTag];
				if (tl.isUndefined())
				{
					qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << bottomRightCornerTag
						<< "' in the '" << cornersTag << "' object";
					return false;
				}
				if (!getJSONPointValue(file, br, bottomRightCornerTag, bottomright))
					return false;

				if (!getJSONPointValue(file, doc, sizeTag, size))
					return false;

				QFileInfo info(file);
				auto field = std::make_shared<GameField>(info.dir(), title_value, image_value, units_value, topleft, bottomright, size);
				fields_.push_back(field);
				return true;
			}
		}
	}
}
