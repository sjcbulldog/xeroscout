//
// Copyright 2020 by Jack W. (Butch) Griffin
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
#include "ChoiceItemDisplay.h"
#include <QStringList>
#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ChoiceFormItem : public FormItemDesc
			{
			public:
				ChoiceFormItem(const QString& display, const QString& tag, const QStringList& choices) : FormItemDesc(display, tag) 
				{
					choices_ = choices;
					addField(std::make_shared<FieldDesc>(tag, choices, true));
				}

				virtual ~ChoiceFormItem() {
				}

				const QStringList& choices() const {
					return choices_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new ChoiceItemDisplay(images, this, parent);
				}

			private:
				QStringList choices_;
			};

		}
	}
}
