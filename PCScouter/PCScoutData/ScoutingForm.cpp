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

#include "ScoutingForm.h"
#include "BooleanFormItem.h"
#include "ChoiceFormItem.h"
#include "UpDownFormItem.h"
#include "NumericFormItem.h"
#include "TextFormItem.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			ScoutingForm::ScoutingForm(const QString& filename)
			{
				parsed_ok_ = false;
				parse(filename);
			}

			ScoutingForm::ScoutingForm(const QJsonObject& obj)
			{
				obj_ = obj;
				parseObject();
			}

			ScoutingForm::~ScoutingForm()
			{
			}

			std::shared_ptr<FormItem> ScoutingForm::itemByName(const QString& name) const
			{
				for (auto section : sections_)
				{
					auto item = section->itemByName(name);
					if (item != nullptr)
						return item;
				}

				return nullptr;
			}

			std::vector<std::pair<QString, QVariant::Type>> ScoutingForm::fields() const
			{
				std::vector<std::pair<QString, QVariant::Type>> result;

				for (auto s : sections_) {
					for (auto i : s->items())
					{
						result.push_back(std::make_pair(i->tag(), i->dataType()));
					}
				}
				return result;
			}

			void ScoutingForm::findLine(int charoff, int& lineno, int& charno)
			{
				lineno = 1;
				charno = 1;

				for (int i = 0; i < charoff; i++) {
					if (json_descriptor_[i] == '\n') {
						lineno++;
						charno = 1;
					}
				}
			}

			void ScoutingForm::parse(const QString& filename)
			{
				QFile file(filename);

				if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
					parsed_ok_ = false;
					return;
				}

				json_descriptor_ = file.readAll();

				QJsonParseError err;
				QJsonDocument doc = QJsonDocument::fromJson(json_descriptor_.toUtf8(), &err);
				if (doc.isNull())
				{
					int lineno;
					int charno;

					findLine(err.offset, lineno, charno);
					errors_.push_back("line " + QString::number(lineno) + ", char " + QString::number(charno) + ": " + err.errorString());
					return;
				}

				if (!doc.isObject())
				{
					errors_.push_back("expected top level JSON entity to be an object");
					return;
				}

				obj_ = doc.object();
				parseObject();
			}

			void ScoutingForm::parseObject()
			{
				if (!obj_.contains("form") || !obj_.value("form").isString()) {
					errors_.push_back("top level JSON object does not contain the string field 'form'");
					return;
				}

				if (!obj_.contains("sections") || !obj_.value("sections").isArray()) {
					errors_.push_back("top level JSON object does not contain the array field 'sections'");
					return;
				}

				form_type_ = obj_.value("form").toString();
				if (form_type_ != "pit" && form_type_ != "match") {
					errors_.push_back("form type ('form' field) is not 'pit' nor 'match'");
					return;
				}

				QJsonArray sections = obj_.value("sections").toArray();
				for (int i = 0; i < sections.size(); i++) {
					if (!sections[i].isObject()) {
						errors_.push_back("entry " + QString::number(i) + " is not a JSON object");
						return;
					}

					if (!parseSection(sections[i].toObject(), i))
						return;
				}

				parsed_ok_ = true;
			}

			bool ScoutingForm::parseSection(const QJsonObject& obj, int index)
			{
				if (!obj.contains("name") || !obj.value("name").isString())
				{
					errors_.push_back("in 'sections' array, entry " + QString::number(index) + ", the required 'name' field is missing or is not a string value");
					return false;
				}

				if (!obj.contains("items") || !obj.value("items").isArray())
				{
					errors_.push_back("in 'sections' array, entry " + QString::number(index) + ", the required 'items' field is missing or is not an array value");
					return false;
				}

				QString sectname = obj.value("name").toString();
				std::shared_ptr<FormSection> section = std::make_shared<FormSection>(sectname);
				sections_.push_back(section);

				QJsonArray arr = obj.value("items").toArray();

				for (int i = 0; i < arr.size(); i++) {
					if (!arr[i].isObject()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
							+ " is not a JSONObject, invalid JSON format");
						return false;
					}

					QJsonObject obj = arr[i].toObject();
					QString type, tag, name;

					if (!obj.contains("type") || !obj.value("type").isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
							+ " there is not string field 'type' which is required");
						return false;
					}

					if (!obj.contains("tag") || !obj.value("tag").isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
							+ " there is not string field 'tag' which is required");
						return false;
					}

					if (!obj.contains("name") || !obj.value("name").isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
							+ " there is not string field 'name' which is required");
						return false;
					}

					type = obj.value("type").toString();
					tag = obj.value("tag").toString();
					name = obj.value("name").toString();

					if (type == "boolean") {
						section->addItem(std::make_shared<BooleanFormItem>(name, tag));
					}
					else if (type == "updown") {
						int minv, maxv;
						if (!obj.contains("minimum") || !obj.value("minimum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
								+ " type type 'updown' requires a double named 'minimum' as a field");
							return false;
						}

						if (!obj.contains("maximum") || !obj.value("maximum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
								+ " type type 'updown' requires a double named 'maximum' as a field");
							return false;
						}

						minv = obj.value("minimum").toInt();
						maxv = obj.value("maximum").toInt();
						section->addItem(std::make_shared<UpDownFormItem>(name, tag, minv, maxv));
					}
					else if (type == "numeric") {
						int minv, maxv;
						if (!obj.contains("minimum") || !obj.value("minimum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
								+ " type type 'numeric' requires a double named 'minimum' as a field");
							return false;
						}

						if (!obj.contains("maximum") || !obj.value("maximum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
								+ " type type 'numeric' requires a double named 'maximum' as a field");
							return false;
						}

						minv = obj.value("minimum").toInt();
						maxv = obj.value("maximum").toInt();

						section->addItem(std::make_shared<NumericFormItem>(name, tag, minv, maxv));
					}
					else if (type == "text") {
						int maxlen = std::numeric_limits<int>::max();
						if (obj.contains("maxlen") && obj.value("maxlen").isDouble())
							maxlen = obj.value("maxlen").toInt();

						section->addItem(std::make_shared<TextFormItem>(name, tag, maxlen));
					}
					else if (type == "choice") {
						if (!obj.contains("choices") || !obj.value("choices").isArray()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
								+ " type type 'choices' requires an array named 'choices' as a field");
							return false;
						}
						QStringList choices;

						QJsonArray charr = obj.value("choices").toArray();
						for (int i = 0; i < charr.size(); i++) {
							if (!charr[i].isString()) {
								errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
									+ " the 'choices' array must contain only strings");
								return false;
							}

							choices.push_back(charr[i].toString());
						}

						section->addItem(std::make_shared<ChoiceFormItem>(name, tag, choices));
					}
					else {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(i + 1)
							+ " the type is invalid, got '" + type + "', expected 'updown' or 'choice' or 'numeric' or 'boolean'");
						return false;
					}
				}
				return true;
			}
		}
	}
}
