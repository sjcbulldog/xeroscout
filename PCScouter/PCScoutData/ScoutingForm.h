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

#include "pcscoutdata_global.h"
#include "FormSection.h"
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>
#include <list>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT ScoutingForm
			{
			public:
				ScoutingForm(const QString& jsonfile);
				ScoutingForm(const QJsonObject& obj);
				virtual ~ScoutingForm();

				const QString& formType() const {
					return form_type_;
				}

				bool isOK() const {
					return parsed_ok_;
				}

				const QStringList& errors() const {
					return errors_;
				}

				const QString& getJSON() const {
					return json_descriptor_;
				}

				std::list<std::shared_ptr<const FormSection>> sections() const {
					std::list<std::shared_ptr<const FormSection>> ret;

					for (auto s : sections_)
						ret.push_back(s);

					return ret;
				}

				const QJsonObject& obj() const {
					return obj_;
				}

				std::vector<std::shared_ptr<FieldDesc>> fields() const;

				std::shared_ptr<FormItemDesc> itemByName(const QString& name) const;

				QStringList images() const;

			private:
				void parse(const QString& filename);
				bool parseSection(const QJsonObject& obj, int index);
				void findLine(int charoff, int& lineno, int& charno);
				void parseObject();
				bool checkDuplicates();
				bool checkFieldNames();

				bool parseSubitemBounds(const QString& secname, int i, const QJsonObject& obj, QRect& r);
				bool parseObjectBase(const QString& secname, int i, const QJsonObject& obj, QString& name, QString& tag);
				std::shared_ptr<FormItemDesc> parseBoolean(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseTimerCounter(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseUpDown(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseChoice(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseNumeric(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseText(const QString& sectname, int i, const QJsonObject& obj);
				std::shared_ptr<FormItemDesc> parseImage(const QString& sectname, int i, const QJsonObject& obj);

				bool isStringArray(const QJsonArray& array) {
					for (int i = 0; i < array.size(); i++) {
						if (!array[i].isString())
							return false;
					}
					return true;
				}

			private:
				QStringList errors_;
				bool parsed_ok_;
				QString json_descriptor_;
				std::list<std::shared_ptr<FormSection>> sections_;
				QString form_type_;
				QJsonObject obj_;

				static QStringList reserved_words_;
			};

		}
	}
}
