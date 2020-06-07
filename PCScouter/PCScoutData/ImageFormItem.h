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
