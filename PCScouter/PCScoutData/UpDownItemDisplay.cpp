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

#include "UpDownItemDisplay.h"
#include "UpDownFormItem.h"
#include <QEvent>
#include <QIntValidator>
#include <QFont>
#include <QFontMetrics>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			UpDownItemDisplay::UpDownItemDisplay(const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(desc, parent)
			{
				label_ = new QLabel(desc->display(), this);

				up_ = new QPushButton("-", this);
				(void)connect(up_, &QPushButton::pressed, this, &UpDownItemDisplay::upButtonPressed);

				down_ = new QPushButton("+", this);
				(void)connect(down_, &QPushButton::pressed, this, &UpDownItemDisplay::downButtonPressed);

				editor_ = new QLineEdit(this);

				QFont font = editor_->font();
				font.setPointSizeF(20.0);
				editor_->setFont(font);

				font.setPointSizeF(16.0);
				setFont(font);

				QFontMetrics fm(font);
				int width = qMax(150, fm.horizontalAdvance(label_->text()));
				setMinimumSize(QSize(width, 90));

				doLayout();

				const UpDownFormItem* fdesc = dynamic_cast<const UpDownFormItem*>(desc);

				editor_->setText(QString::number(fdesc->minValue()));
				editor_->setValidator(new QIntValidator(fdesc->minValue(), fdesc->maxValue(), this));
			}

			UpDownItemDisplay::~UpDownItemDisplay()
			{
			}

			void UpDownItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				
				editor_->setText(QString::number(data.data(0).toInt()));
			}

			DataCollection UpDownItemDisplay::getValues()
			{
				DataCollection d;

				d.add(desc()->tag(), editor_->text().toInt());
				return d;
			}

			int UpDownItemDisplay::value() const {
				return editor_->text().toInt();
			}

			void UpDownItemDisplay::upButtonPressed()
			{
				const UpDownFormItem* fdesc = dynamic_cast<const UpDownFormItem*>(desc());
				int v = value() - 1;
				if (v >= fdesc->minValue() && v <= fdesc->maxValue())
					editor_->setText(QString::number(v));
			}

			void UpDownItemDisplay::downButtonPressed()
			{
				const UpDownFormItem* fdesc = dynamic_cast<const UpDownFormItem*>(desc());
				int v = value() + 1;
				if (v >= fdesc->minValue() && v <= fdesc->maxValue())
					editor_->setText(QString::number(v));
			}

			void UpDownItemDisplay::changeEvent(QEvent* ev)
			{
				if (ev->type() == QEvent::FontChange)
					doLayout();
			}

			void UpDownItemDisplay::resizeEvent(QResizeEvent* ev)
			{
				doLayout();
			}

			void UpDownItemDisplay::doLayout()
			{
				int button_width = 30;
				int button_height = 30;

				QFontMetrics fm(font());

				QRect r = QRect(0, 0, width(), fm.height());
				label_->setGeometry(r);

				int remaining = height() - fm.height();
				r = QRect(width() - button_width, fm.height(), button_width, remaining / 2);
				down_->setGeometry(r);

				r = QRect(width() - button_width, fm.height() + remaining / 2, button_width, remaining / 2);
				up_->setGeometry(r);

				r = QRect(0, fm.height(), width() - button_width, height() - fm.height());
				editor_->setGeometry(r);
			}
		}
	}
}
