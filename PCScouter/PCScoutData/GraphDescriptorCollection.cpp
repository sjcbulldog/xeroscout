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

