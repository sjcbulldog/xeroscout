#pragma once
#include "FormItemDesc.h"
#include "ImageFormSubItem.h"
#include "ImageItemPublishInfo.h"
#include "ImageItemDisplay.h"
#include <memory>
#include <list>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageFormItem : public FormItemDesc
			{
			public:
				ImageFormItem(const QString& display, const QString& tag, const QString& image, double scale);
				virtual ~ImageFormItem();

				const QString& imageTag() const {
					return image_;
				}

				virtual DataCollection random(GameRandomProfile& profile) const;

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const {
					return new ImageItemDisplay(images, this, parent);
				}

				void addSubItem(std::shared_ptr<ImageFormSubItem> item);

				void addPublishItem(std::shared_ptr<ImageItemPublishInfo> item);

				double scale() const {
					return scale_;
				}

				std::list<std::shared_ptr<ImageFormSubItem>> items() const {
					return sub_items_;
				}

				std::list<std::shared_ptr<ImageItemPublishInfo>> published() const {
					return publish_items_;
				}

				void addPublishedItems(DataCollection& d) const;

			private:
				std::list<std::shared_ptr<ImageFormSubItem>> sub_items_;
				std::list<std::shared_ptr<ImageItemPublishInfo>> publish_items_;
				QString image_;
				double scale_;
			};
		}
	}
}
