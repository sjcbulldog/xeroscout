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

#include "NumericItemDisplay.h"
#include "NumericFormItem.h"
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QIntValidator>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			NumericItemDisplay::NumericItemDisplay(ImageSupplier &images, const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				QHBoxLayout* layout = new QHBoxLayout();
				setLayout(layout);

				QLabel* label = new QLabel(desc->display(), this);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				auto ndesc = dynamic_cast<const NumericFormItem *>(desc);

				edit_ = new QLineEdit(this);
				edit_->setValidator(new QIntValidator(ndesc->minValue(), ndesc->maxValue(), parent));
				layout->addWidget(edit_);
				font = edit_->font();
				font.setPointSizeF(16.0);
				edit_->setFont(font);

				edit_->setText(QString::number(ndesc->minValue()));
			}

			NumericItemDisplay::~NumericItemDisplay()
			{
			}

			void NumericItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				edit_->setText(QString::number(data.data(0).toInt()));
			}

			DataCollection NumericItemDisplay::getValues()
			{
				DataCollection d;

				int v = edit_->text().toInt();
				d.add(desc()->tag(), QVariant(v));

				return d;
			}
		}
	}
}
