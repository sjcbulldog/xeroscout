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
				label->setToolTip(desc->tag());

				auto ndesc = dynamic_cast<const NumericFormItem *>(desc);

				edit_ = new QLineEdit(this);
				edit_->setValidator(new QIntValidator(ndesc->minValue(), ndesc->maxValue(), parent));
				layout->addWidget(edit_);
				font = edit_->font();
				font.setPointSizeF(16.0);
				edit_->setFont(font);
				edit_->setToolTip(desc->tag());

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
