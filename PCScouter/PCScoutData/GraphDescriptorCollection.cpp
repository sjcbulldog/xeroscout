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

#include "GraphDescriptorCollection.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			GraphDescriptorCollection::GraphDescriptorCollection()
			{
			}

			GraphDescriptorCollection::~GraphDescriptorCollection()
			{
			}

			void GraphDescriptorCollection::update(const GraphDescriptor& desc)
			{
				this->insert_or_assign(desc.name(), desc);
			}

			QJsonArray GraphDescriptorCollection::generateJSON() const
			{
				QJsonArray array;

				for (auto pair : *this)
				{
					QJsonObject obj = pair.second.generateJSON();
					array.push_back(obj);
				}

				return array;
			}

			bool GraphDescriptorCollection::load(const QJsonArray &array, QString& errstr)
			{
				clear();

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
					{
						errstr = "entry " + QString::number(i + 1) + ": entry is not a JSON object";
						return false;
					}

					if (!readDesc(i + 1, array[i].toObject(), errstr))
						return false;
				}

				return true;
			}

			bool GraphDescriptorCollection::isArrayString(const QJsonArray& array)
			{
				for (auto a : array)
				{
					if (!a.isString())
						return false;
				}

				return true;
			}

			bool GraphDescriptorCollection::isArrayDouble(const QJsonArray& array)
			{
				for (auto a : array)
				{
					if (!a.isDouble())
						return false;
				}

				return true;
			}

			bool GraphDescriptorCollection::readDesc(int entry, const QJsonObject& obj, QString& errstr)
			{
				if (!obj.contains("name"))
				{
					errstr = "entry " + QString::number(entry) + ": object does not contain field 'name'" ;
					return false;
				}

				if (!obj.value("name").isString())
				{
					errstr = "entry " + QString::number(entry) + ": object contains field 'name', but it is not a string";
					return false;
				}

				if (!obj.contains("panes"))
				{
					errstr = "entry " + QString::number(entry) + ": object does not contain field 'panes'";
					return false;
				}

				if (!obj.value("panes").isArray())
				{
					errstr = "entry " + QString::number(entry) + ": object contains field 'title', but it is not an array";
					return false;
				}

				GraphDescriptor desc (obj.value("name").toString());

				if (!readPanes(entry, desc, obj.value("panes").toArray(), errstr))
					return false;

				insert_or_assign(obj.value("name").toString(), desc);

				return true;
			}

			bool GraphDescriptorCollection::readPanes(int entry, GraphDescriptor &desc, const QJsonArray& array, QString& errstr)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
					{
						errstr = "entry " + QString::number(entry) + ": panes array does not contain all objects";
						return false;
					}

					const QJsonObject& obj = array[i].toObject();
					if (!obj.contains("title"))
					{
						errstr = "entry " + QString::number(entry) + ": panes array object does not contain field 'title'";
						return false;
					}

					if (!obj.value("title").isString())
					{
						errstr = "entry " + QString::number(entry) + ": panes array object contains field 'title', but it is not a string";
						return false;
					}

					if (!obj.contains("x"))
					{
						errstr = "entry " + QString::number(entry) + ": panes array object does not contain field 'x'";
						return false;
					}

					if (!obj.value("x").isArray() || !isArrayString(obj.value("x").toArray()))
					{
						errstr = "entry " + QString::number(entry) + ": panes array object contains field 'x', but it is not a string array";
						return false;
					}

					if (!obj.contains("range"))
					{
						errstr = "entry " + QString::number(entry) + ": panes array object does not contain field 'range'";
						return false;
					}

					if (!obj.value("range").isArray() || !isArrayDouble(obj.value("range").toArray()))
					{
						errstr = "entry " + QString::number(entry) + ": panes array object contains field 'range', but it is not a double array";
						return false;
					}

					if (obj.value("range").toArray().size() != 2)
					{
						errstr = "entry " + QString::number(entry) + ": panes array object contains field 'range', but it is not size 2 (min, max)";
						return false;
					}

					auto pane = desc.addPane(obj.value("title").toString());
					QJsonArray& x = obj.value("x").toArray();
					for (const QJsonValue& xitem : x)
						pane->addX(xitem.toString());

					auto array = obj.value("range").toArray();
					pane->setRange(array[0].toDouble(), array[1].toDouble());
				}

				return true;
			}
		}
	}
}

