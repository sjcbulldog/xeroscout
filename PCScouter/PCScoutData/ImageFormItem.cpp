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

#include "ImageFormItem.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ImageFormItem::ImageFormItem(const QString& display, const QString& tag, const QString &image, double scale) : FormItemDesc(display, tag)
			{
				scale_ = scale;
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
