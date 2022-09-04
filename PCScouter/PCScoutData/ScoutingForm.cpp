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

#include "ScoutingForm.h"
#include "BooleanFormItem.h"
#include "ChoiceFormItem.h"
#include "UpDownFormItem.h"
#include "NumericFormItem.h"
#include "TextFormItem.h"
#include "TimerCountFormItem.h"
#include "ImageFormItem.h"
#include "ImageFormCountSubItem.h"
#include "ImageFormOnOffSubItem.h"
#include "ImageItemPublishInfo.h"
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonParseError>
#include <QTextCodec>
#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			QStringList ScoutingForm::reserved_words_ =
			{
				"ABORT", "ACTION", "ABORT", "ACTION", "ADD","AFTER","ALL", "ALTER", "ALWAYS", "ANALYZE", "AND", "AS", "ASC", "ATTACH", "AUTOINCREMENT", "BEFORE", "BEGIN", "BETWEEN",
				"BY", "CASCADE", "CASE", "CAST", "CHECK", "COLLATE", "COLUMN", "COMMIT", "CONFLICT", "CONSTRAINT", "CREATE", "CROSS", "CURRENT", "CURRENT_DATE", "CURRENT_TIME",
				"CURRENT_TIMESTAMP", "DATABASE", "DEFAULT", "DEFERRABLE", "DEFERRED", "DELETE", "DESC", "DETACH", "DISTINCT", "DO", "DROP", "EACH", "ELSE", "END", "ESCAPE", "EXCEPT",
				"EXCLUDE", "EXCLUSIVE", "EXISTS", "EXPLAIN", "FAIL", "FILTER", "FIRST", "FOLLOWING", "FOR", "FOREIGN", "FROM", "FULL", "GENERATED", "GLOB", "GROUP", "GROUPS", "HAVING",
				"IF","IGNORE", "IMMEDIATE", "IN", "INDEX", "INDEXED", "INITIALLY", "INNER", "INSERT", "INSTEAD", "INTERSECT", "INTO", "IS", "ISNULL", "JOIN", "KEY", "LAST", "LEFT",
				"LIKE", "LIMIT", "MATCH", "NATURAL", "NO", "NOT", "NOTHING", "NOTNULL", "NULL", "NULLS", "OF", "OFFSET", "ON", "OR", "ORDER", "OTHERS", "OUTER", "OVER", "PARTITION",
				"PLAN","PRAGMA", "PRECEDING", "PRIMARY", "QUERY", "RAISE", "RANGE", "RECURSIVE", "REFERENCES", "REGEXP", "REINDEX", "RELEASE", "RENAME", "REPLACE", "RESTRICT", "RIGHT",
				"ROLLBACK", "ROW", "ROWS", "SAVEPOINT", "SELECT", "SET", "TABLE", "TEMP", "TEMPORARY", "THEN", "TIES", "TO", "TRANSACTION", "TRIGGER", "UNBOUNDED", "UNION", "UNIQUE",
				"UPDATE", "USING", "VACUUM", "VALUES", "VIEW", "VIRTUAL", "WHEN", "WHERE", "WINDOW", "WITH", "WITHOUT"
			};

			ScoutingForm::ScoutingForm()
			{
				parsed_ok_ = true;
			}

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

			bool ScoutingForm::addSection(const QString& name)
			{
				for (auto s : sections_)
				{
					if (s->name() == name)
						return false;
				}

				sections_.push_back(std::make_shared<FormSection>(name));
				return true;
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::itemByName(const QString& name) const
			{
				for (auto section : sections_)
				{
					auto item = section->itemByName(name);
					if (item != nullptr)
						return item;
				}

				return nullptr;
			}

			std::vector<std::shared_ptr<FieldDesc>> ScoutingForm::fields() const
			{
				std::vector<std::shared_ptr<FieldDesc>> result;

				for (auto s : sections_) {
					for (auto i : s->items())
					{
						//
						// We exclude the blue versions because the red and blue should be symmetrical and
						// we want the unique field names.
						//
						if (i->alliance() == Alliance::Both || i->alliance() == Alliance::Red)
						{
							auto fields = i->getFields();
							result.insert(result.end(), fields.begin(), fields.end());
						}
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
				if (form_type_ != "team" && form_type_ != "match") {
					errors_.push_back("form type ('form' field) is not 'team' nor 'match'");
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

				if (!checkDuplicates())
					return;

				parsed_ok_ = true;
			}

			bool ScoutingForm::checkFieldNames()
			{
				bool ret = true;

				for (auto s : sections_)
				{
					int entry = 0;
					for (auto i : s->items())
					{
						entry++;

						const QString& tag = i->tag();
						if (tag.length() == 0) {
							errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
								+ " the tag field is an empty string");
							ret = false;
						}

						if (!tag.at(0).isLetter()) {
							errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
								+ " the tag field must start with a letter");
							ret = false;
						}

						for (int i = 1; i < tag.length(); i++)
						{
							if (!tag.at(i).isLetterOrNumber() && tag.at(i) != '_') {
								errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
									+ " the tag field must start with a letter and consists of letters, numbers, and '_'");
								ret =  false;
							}
						}

						if (tag.startsWith("ba_")) {
							errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
								+ " item with tag '" + tag + "' starts with 'ba_' which is reserved for blue alliance data");
							return false;
						}

						if (tag.startsWith("calc_")) {
							errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
								+ " item with tag '" + tag + "' starts with 'calc_' which is reserved for computed data");
							return false;
						}

						if (reserved_words_.contains(tag.toUpper()))
						{
							errors_.push_back("in section '" + s->name() + "', entry " + QString::number(entry)
								+ " the tag field '" + tag + "' is a reserved keyword");
							ret = false;
						}
					}
				}

				return true;
			}


			bool ScoutingForm::checkDuplicates()
			{
				bool ret = true;
				QStringList red, blue;

				for (auto s : sections_)
				{
					for (auto i : s->items())
					{
						if (i->alliance() == Alliance::Both)
						{
							if (red.contains(i->tag()))
							{
								errors_.push_back("item with tag '" + i->tag() + "' is duplicated for red alliance");
								ret = false;
							}
							else
							{
								red.push_back(i->tag());
							}

							if (blue.contains(i->tag()))
							{
								errors_.push_back("item with tag '" + i->tag() + "' is duplicated for blue alliance");
								ret = false;
							}
							else
							{
								blue.push_back(i->tag());
							}
						}
						else if (i->alliance() == Alliance::Red)
						{
							if (red.contains(i->tag()))
							{
								errors_.push_back("item with tag '" + i->tag() + "' is duplicated for red alliance");
								ret = false;
							}
							else
							{
								red.push_back(i->tag());
							}
						}
						else if (i->alliance() == Alliance::Blue)
						{
							if (blue.contains(i->tag()))
							{
								errors_.push_back("item with tag '" + i->tag() + "' is duplicated for blue alliance");
								ret = false;
							}
							else
							{
								blue.push_back(i->tag());
							}

						}
					}
				}
				return ret;
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
					int entry = i + 1;
					std::shared_ptr<FormItemDesc> item;

					if (!arr[i].isObject()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " is not a JSONObject, invalid JSON format");
						return false;
					}

					QJsonObject obj = arr[i].toObject();
					QString type, alliance;

					if (!obj.contains("type") || !obj.value("type").isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " there is no string field 'type' which is required");
						return false;
					}
					type = obj.value("type").toString();

					if (obj.contains("alliance") && !obj.value("alliance").isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " there is an 'alliance' field but it is not a string");
						return false;
					}

					if (obj.contains("alliance"))
					{
						alliance = obj.value("alliance").toString();
						if (alliance != "red" && alliance != "blue")
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " there is an 'alliance' field but it is 'red' or 'blue'");
							return false;
						}
					}

					if (type == "boolean") {
						item = parseBoolean(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "timercnt") {
						item = parseTimerCounter(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "updown") {
						item = parseUpDown(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "numeric") {
						item = parseNumeric(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "text") {
						item = parseText(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "choice") {
						item = parseChoice(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else if (type == "image") {
						item = parseImage(sectname, entry, obj);
						if (item == nullptr)
							return false;

						section->addItem(item);
					}
					else {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " the type is invalid, got '" + type + "', expected 'updown' or 'choice' or 'numeric' or 'boolean'");
						return false;
					}

					Alliance a = Alliance::Both;
					
					if (alliance.length() > 0)
						a = allianceFromString(alliance);

					if (a == Alliance::Invalid)
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " the alliance is invalid, got '" + alliance + "', expected 'red' or 'blue'");
						return false;
					}
					item->setAlliance(a);
				}
				return true;
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseBoolean(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QString name, tag;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				return std::make_shared<BooleanFormItem>(name, tag);
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseTimerCounter(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QString name, tag;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				int minv, maxv;
				if (!obj.contains("minimum") || !obj.value("minimum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'timercnt' requires a double named 'minimum' as a field");
					return nullptr;
				}

				if (!obj.contains("maximum") || !obj.value("maximum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'timercnt' requires a double named 'maximum' as a field");
					return nullptr;
				}

				minv = obj.value("minimum").toInt();
				maxv = obj.value("maximum").toInt();
				return std::make_shared<TimerCountFormItem>(name, tag, minv, maxv);
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseUpDown(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QString name, tag ;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				int minv, maxv;
				if (!obj.contains("minimum") || !obj.value("minimum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'updown' requires a double named 'minimum' as a field");
					return nullptr;
				}

				if (!obj.contains("maximum") || !obj.value("maximum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'updown' requires a double named 'maximum' as a field");
					return nullptr;
				}

				minv = obj.value("minimum").toInt();
				maxv = obj.value("maximum").toInt();
				return std::make_shared<UpDownFormItem>(name, tag, minv, maxv);
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseChoice(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QString name, tag;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				if (!obj.contains("choices") || !obj.value("choices").isArray()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'choices' requires an array named 'choices' as a field");
					return nullptr;
				}
				QStringList choices;

				QJsonArray charr = obj.value("choices").toArray();
				for (int i = 0; i < charr.size(); i++) {
					if (!charr[i].isString()) {
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " the 'choices' array must contain only strings");
						return nullptr;
					}

					choices.push_back(charr[i].toString());
				}

				return std::make_shared<ChoiceFormItem>(name, tag, choices);
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseNumeric(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QString name, tag;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				int minv, maxv;
				if (!obj.contains("minimum") || !obj.value("minimum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'numeric' requires a double named 'minimum' as a field");
					return nullptr;
				}

				if (!obj.contains("maximum") || !obj.value("maximum").isDouble()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " type type 'numeric' requires a double named 'maximum' as a field");
					return nullptr;
				}

				minv = obj.value("minimum").toInt();
				maxv = obj.value("maximum").toInt();

				std::shared_ptr<NumericFormItem> item;

				if (obj.contains("minlimit") && obj.value("minlimit").isDouble() && obj.contains("maxlimit") && obj.value("maxlimit").isDouble())
				{
					int minlim = obj.value("minlimit").toInt();
					int maxlim = obj.value("maxlimit").toInt();
					item = std::make_shared<NumericFormItem>(name, tag, minv, maxv, minlim, maxlim);
				}
				else
				{
					item = std::make_shared<NumericFormItem>(name, tag, minv, maxv);
				}
				return item;
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseText(const QString& sectname, int i, const QJsonObject& obj)
			{
				QString name, tag;
				if (!parseObjectBase(sectname, i, obj, name, tag))
					return nullptr;

				int maxlen = std::numeric_limits<int>::max();
				if (obj.contains("maxlen") && obj.value("maxlen").isDouble())
					maxlen = obj.value("maxlen").toInt();

				return std::make_shared<TextFormItem>(name, tag, maxlen);
			}

			std::shared_ptr<FormItemDesc> ScoutingForm::parseImage(const QString& sectname, int entry, const QJsonObject& obj)
			{
				QJsonDocument doc;
				doc.setObject(obj);
				QByteArray Data = doc.toJson(QJsonDocument::JsonFormat::Indented);
				QString DataAsString = QTextCodec::codecForMib(106)->toUnicode(Data);

				QString name, tag;
				if (!parseObjectBase(sectname, entry, obj, name, tag))
					return nullptr;

				if (!obj.contains("image") || !obj.value("image").isString())
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no string field 'image' which is required");
					return nullptr;
				}

				if (!obj.contains("scale") || !obj.value("scale").isDouble())
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no string field 'scale' which is required");
					return nullptr;
				}

				if (!obj.contains("imageitems") || !obj.value("imageitems").isArray())
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no array field 'imageitems' which is required");
					return nullptr;
				}

				auto image = std::make_shared<ImageFormItem>(name, tag, obj.value("image").toString(), obj.value("scale").toDouble());

				QJsonArray array = obj.value("imageitems").toArray();
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " element in 'imageitems' array is not a json object");
						return nullptr;
					}

					QJsonObject iobj = array[i].toObject();
					QStringList keys = iobj.keys();
					if (!iobj.contains("imageitemtype") || !iobj.value("imageitemtype").isString())
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " there is no string field 'imageitemtype' which is required");
						return nullptr;
					}

					if (!iobj.contains("tag") || !iobj.value("tag").isString())
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " there is no string field 'tag' which is required");
						return nullptr;
					}

					QRect r;
					QString type = iobj.value("imageitemtype").toString();

					if (type == "choice")
					{
						if (!iobj.contains("value") || !iobj.value("value").isString())
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " there is no string field 'tag' which is required");
							return nullptr;
						}

						if (!parseSubitemBounds(sectname, entry, iobj, r))
							return nullptr;

						auto subitem = std::make_shared<ImageFormOnOffSubitem>(iobj.value("tag").toString(), iobj.value("value").toString(), r);
						image->addSubItem(subitem);
					}
					else if (type == "onoff")
					{
						if (iobj.contains("value")) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " the subitem type is 'onoff' but there is a 'value' field.  The value field is only valid for 'choice' subitem types.");
							return nullptr;
						}
						if (!parseSubitemBounds(sectname, entry, iobj, r))
							return nullptr;

						auto subitem = std::make_shared<ImageFormOnOffSubitem>(iobj.value("tag").toString(), r);
						image->addSubItem(subitem);
					}
					else if (type == "count")
					{
						QString color;

						if (!parseSubitemBounds(sectname, entry, iobj, r))
							return nullptr;

						QStringList keys = iobj.keys();
						int minv, maxv;
						if (!iobj.contains("minimum") || !iobj.value("minimum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " subtype item 'count' requires a double named 'minimum' as a field");
							return nullptr;
						}

						if (!iobj.contains("maximum") || !iobj.value("maximum").isDouble()) {
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " subtype item 'count' requires a double named 'maximum' as a field");
							return nullptr;
						}

						minv = iobj.value("minimum").toInt();
						maxv = iobj.value("maximum").toInt();

						if (iobj.contains("color") && iobj.value("color").isString())
							color = iobj.value("color").toString();

						if (iobj.contains("minlimit") && iobj.value("minlimit").isDouble() && iobj.contains("maxlimit") && iobj.value("maxlimit").isDouble())
						{
							int minlim = iobj.value("minlimit").toInt();
							int maxlim = iobj.value("maxlimit").toInt();
							auto subitem = std::make_shared<ImageFormCountSubItem>(iobj.value("tag").toString(), color, r, minv, maxv, minlim, maxlim);
							image->addSubItem(subitem);
						}
						else
						{
							auto subitem = std::make_shared<ImageFormCountSubItem>(iobj.value("tag").toString(), color, r, minv, maxv);
							image->addSubItem(subitem);
						}

					}
				}

				QStringList keys = obj.keys();
				if (obj.contains("publishitems"))
				{
					if (!obj.value("publishitems").isArray())
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " the optional field 'publishitems' when present is required to be an array");
						return nullptr;
					}

					array = obj.value("publishitems").toArray();
					for (int i = 0; i < array.size(); i++)
					{
						if (!array[i].isObject())
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " element in 'imageitems' array is not a json object");
							return nullptr;
						}

						QJsonObject iobj = array[i].toObject();

						if (!iobj.contains("tag") || !iobj.value("tag").isString())
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " there is no string field 'tag' which is required");
							return nullptr;
						}

						if (!iobj.contains("type") || !iobj.value("type").isString())
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " there is no string field 'type' which is required");
							return nullptr;
						}
						QString typestr = iobj.value("type").toString();
						if (typestr != "sum" && typestr != "average")
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " the 'type' field must be either 'sum' or 'average'");
							return nullptr;
						}

						if (!iobj.contains("imageitems") || !iobj.value("imageitems").isArray() || !isStringArray(iobj.value("imageitems").toArray()))
						{
							errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
								+ " there is no string array field 'imageitems' which is required");
							return nullptr;
						}

						QJsonArray array = iobj.value("imageitems").toArray();
						ImageItemPublishInfo::PublishType ptype = ImageItemPublishInfo::PublishType::Average;
						if (typestr == "sum")
							ptype = ImageItemPublishInfo::PublishType::Sum;
						auto item = std::make_shared<ImageItemPublishInfo>(iobj.value("tag").toString(), ptype);

						for (auto arritem : array)
						{
							item->addComponents(arritem.toString());
						}

						image->addPublishItem(item);
					}
				}

				return image;
			}

			bool ScoutingForm::parseSubitemBounds(const QString& sectname, int entry, const QJsonObject& obj, QRect& r)
			{
				if (!obj.contains("position") || !obj.value("position").isArray())
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no array field 'position' which is required");
					return false;
				}

				QJsonArray array = obj.value("position").toArray();

				if (array.size() != 5)
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " the position field is malformed, it should have five entries, a string followed by four numbers");
					return false;
				}

				if (!array[0].isString())
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " the position field is malformed, it should have five entries, a string followed by four numbers");
					return false;
				}

				QString type = array[0].toString();
				if (type != "centersize" && type != "bounds")
				{
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " the position field is malformed, the first entry should be 'centersize' or 'bounds'");
					return false;
				}

				std::vector<int> nums;
				for (int i = 1; i < array.size(); i++)
				{
					if (!array[i].isDouble())
					{
						errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
							+ " the position field is malformed, it should have five entries, a string followed by four numbers");
						return false;
					}

					nums.push_back(array[i].toInt());
				}

				if (type == "centersize")
				{
					int x = nums[0] - nums[2] / 2;
					int y = nums[1] - nums[3] / 2;
					r = QRect(QPoint(x, y), QSize(nums[2], nums[3]));
				}
				else
				{
					int width = nums[2] - nums[0];
					int height = nums[3] - nums[1];
					r = QRect(QPoint(nums[0], nums[1]), QSize(width, height));
				}

				return true;
			}

			bool ScoutingForm::parseObjectBase(const QString &sectname, int entry, const QJsonObject& obj, QString& name, QString& tag)
			{
				if (!obj.contains("tag") || !obj.value("tag").isString()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no string field 'tag' which is required");
					return false;
				}
				tag = obj.value("tag").toString();


				if (!obj.contains("name") || !obj.value("name").isString()) {
					errors_.push_back("in section '" + sectname + "', entry " + QString::number(entry)
						+ " there is no string field 'name' which is required");
					return false;
				}


				name = obj.value("name").toString();
				return true;
			}

			QStringList ScoutingForm::images() const
			{
				QStringList list;

				for (auto s : sections_)
				{
					for (auto item : s->items())
					{
						std::shared_ptr<const ImageFormItem> im = std::dynamic_pointer_cast<const ImageFormItem>(item);
						if (im != nullptr)
						{
							if (!list.contains(im->imageTag()))
								list.push_back(im->imageTag());
						}
					}
				}

				return list;
			}
		}
	}
}
