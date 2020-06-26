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

#include "pcscoutdata_global.h"
#include "FormSection.h"
#include "DataSetHighlightRules.h"
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
				ScoutingForm();
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

				bool addSection(const QString& name);

				const std::vector<std::shared_ptr<const DataSetHighlightRules>>& rules() const {
					return rules_;
				}

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

				std::vector<std::shared_ptr<const DataSetHighlightRules>> rules_;

				static QStringList reserved_words_;
			};

		}
	}
}
