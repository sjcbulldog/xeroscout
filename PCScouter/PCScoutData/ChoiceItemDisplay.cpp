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
