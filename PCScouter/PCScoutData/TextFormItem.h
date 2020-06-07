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
#include "TextItemDisplay.h"
#include <QString>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class TextFormItem : public FormItemDesc
			{
			public:
				TextFormItem(const QString& display, const QString& tag, int maxlen) : FormItemDesc(display, tag)
				{
					maxlen_ = maxlen;
					addField(std::make_shared<FieldDesc>(tag, FieldDesc::Type::String, true));
				}

				virtual ~TextFormItem()
				{
				}

				int maxLen() const {
					return maxlen_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new TextItemDisplay(images, this, parent);
				}

			private:
				int maxlen_;
			};

		}
	}
}
