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

#include "EditItemDisplay.h"
#include "TextItemDisplay.h"
#include "EditFormItem.h"
#include <QBoxLayout>
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			EditItemDisplay::EditItemDisplay(ImageSupplier& images, const FormItemDesc* desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				QHBoxLayout* layout = new QHBoxLayout();
				setLayout(layout);

				QLabel* label = new QLabel(desc->display(), this);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				auto tdesc = dynamic_cast<const EditFormItem*>(desc);

				edit_ = new QPlainTextEdit(this);
				layout->addWidget(edit_);
				font = edit_->font();
				font.setPointSizeF(16.0);
				edit_->setFont(font);

				QFontMetrics fm(font);
				int height = fm.lineSpacing() * tdesc->rows();
				int width = fm.averageCharWidth() * tdesc->cols();

				edit_->setFixedSize(width, height);
			}

			EditItemDisplay::~EditItemDisplay()
			{
			}

			void EditItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				assert(data.data(0).type() == QVariant::Type::String);

				edit_->setPlainText(data.data(0).toString());
			}

			DataCollection EditItemDisplay::getValues()
			{
				DataCollection d;
				d.add(desc()->tag(), QVariant(edit_->toPlainText()));

				return d;
			}
		}
	}
}
