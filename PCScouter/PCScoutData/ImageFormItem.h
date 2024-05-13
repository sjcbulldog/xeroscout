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
				ImageFormItem(const QString& display, const QString& tag, const QString& image);
				virtual ~ImageFormItem();

				void getImages(QStringList& imlist) {
					imlist.push_back(image_);
				}

				const QString& imageTag() const {
					return image_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const {
					return new ImageItemDisplay(images, this, parent);
				}

				void addSubItem(std::shared_ptr<ImageFormSubItem> item);

				void addPublishItem(std::shared_ptr<ImageItemPublishInfo> item);

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
			};
		}
	}
}
