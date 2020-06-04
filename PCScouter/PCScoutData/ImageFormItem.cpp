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

			DataCollection ImageFormItem::random(GameRandomProfile& profile) const
			{
				QVariant v;
				DataCollection d;
				std::map<QString, QStringList> choices;

				for (auto item : sub_items_)
				{
					QString longname = FormItemDesc::genComplexName(tag(), item->subname());

					auto on_off = std::dynamic_pointer_cast<ImageFormOnOffSubitem>(item);
					if (on_off != nullptr)
					{
						if (on_off->isChoice())
						{
							QStringList list;
							auto it = choices.find(longname);
							if (it != choices.end())
								list = it->second;

							list.push_back(on_off->value());
							choices.insert_or_assign(longname, list);
						}
						else
						{
							v = profile.generateRandomBool(longname);
							d.add(longname, v);
						}
					}

					auto count = std::dynamic_pointer_cast<ImageFormCountSubItem>(item);
					if (count != nullptr)
					{
						v = profile.generateRandomInteger(longname, count->minimum(), count->maximum());
						d.add(longname, v);
					}
				}

				for (auto pair : choices)
				{
					v = profile.generateRandomChoice(pair.first, pair.second);
					d.add(pair.first, v);
				}

				addPublishedItems(d);

				return d;
			}
		}
	}
}
