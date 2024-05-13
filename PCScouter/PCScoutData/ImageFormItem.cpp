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

#include "ImageFormItem.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ImageFormItem::ImageFormItem(const QString& display, const QString& tag, const QString &image) : FormItemDesc(display, tag)
			{
				image_ = image;
			}

			ImageFormItem::~ImageFormItem()
			{
			}

			void ImageFormItem::addSubItem(std::shared_ptr<ImageFormSubItem> item)
			{
				sub_items_.push_back(item);

				QString name = genComplexName(tag(), item->subname());
				auto on_off = std::dynamic_pointer_cast<ImageFormOnOffSubitem>(item);
				if (on_off != nullptr)
				{
					if (on_off->isChoice())
					{
						if (!hasField(name))
						{
							addField(std::make_shared<FieldDesc>(name, FieldDesc::Type::StringChoice, true));
						}

						auto field = findField(name);
						assert(field != nullptr);
						field->addChoice(on_off->value());
					}
					else
					{
						addField(std::make_shared<FieldDesc>(name, FieldDesc::Type::Boolean, true));
					}
				}

				auto count = std::dynamic_pointer_cast<ImageFormCountSubItem>(item);
				if (count != nullptr)
				{
					auto field = std::make_shared<FieldDesc>(name, FieldDesc::Type::Integer, true);
					if (count->hasLimits())
						field->setLimits(count->minLimit(), count->maxLimit());
					addField(field);
				}
			}

			void ImageFormItem::addPublishItem(std::shared_ptr<ImageItemPublishInfo> item)
			{
				publish_items_.push_back(item);

				QString name = genComplexName(tag(), item->tag());
				if (item->type() == ImageItemPublishInfo::PublishType::Average)
					addField(std::make_shared<FieldDesc>(name, FieldDesc::Type::Double, true));
				else
					addField(std::make_shared<FieldDesc>(name, FieldDesc::Type::Integer, true));
			}

			void ImageFormItem::addPublishedItems(DataCollection& d) const
			{
				for (auto publish : published())
				{
					QString pubname = FormItemDesc::genComplexName(tag(), publish->tag());
					int total = 0;
					int count = 0;

					for (const QString item : publish->components())
					{
						QString longname = FormItemDesc::genComplexName(tag(), item);
						if (!d.has(longname))
							continue;

						QVariant v = d.dataByName(longname);
						total += v.toInt();
						count++;
					}

					if (publish->type() == ImageItemPublishInfo::PublishType::Average)
					{
						d.add(pubname, QVariant((double)total / (double)count));
					}
					else
					{
						d.add(pubname, QVariant(total));
					}
				}
			}
		}
	}
}
