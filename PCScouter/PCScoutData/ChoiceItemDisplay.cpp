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

#include "ChoiceItemDisplay.h"
#include "ChoiceFormItem.h"
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ChoiceItemDisplay::ChoiceItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				QHBoxLayout* layout = new QHBoxLayout();
				setLayout(layout);

				QLabel* label = new QLabel(desc->display(), this);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				const ChoiceFormItem* cdesc = dynamic_cast<const ChoiceFormItem*>(desc);
				box_ = new QComboBox(this);
				layout->addWidget(box_);
				for (const QString& choice : cdesc->choices())
					box_->addItem(choice);

				box_->setCurrentIndex(0);

				font = box_->font();
				font.setPointSizeF(16.0);
				box_->setFont(font);
			}

			ChoiceItemDisplay::~ChoiceItemDisplay()
			{
			}

			void ChoiceItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				assert(data.data(0).type() == QVariant::Type::String);

				for (int i = 0; i < box_->count(); i++)
				{
					if (box_->itemText(i) == data.data(0).toString())
					{
						box_->setCurrentIndex(i);
						break;
					}
				}
			}

			DataCollection ChoiceItemDisplay::getValues()
			{
				DataCollection d;
				d.add(desc()->tag(), QVariant(box_->currentText()));
				return d;
			}
		}
	}
}
