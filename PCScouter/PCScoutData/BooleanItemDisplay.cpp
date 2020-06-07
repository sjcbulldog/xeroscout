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



#include "BooleanItemDisplay.h"
#include "DataCollection.h"
#include <QPainter>
#include <QFontMetrics>
#include <QEvent>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			BooleanItemDisplay::BooleanItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				QFontMetrics fm(font());
				int width = fm.horizontalAdvance(desc->display());
				setMinimumSize(QSize(width + 8, 80));

				state_ = false;
			}

			BooleanItemDisplay::~BooleanItemDisplay()
			{
			}

			void BooleanItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				assert(data.data(0).type() == QVariant::Type::Bool);

				if (data.data(0).toBool())
					state_ = true;
				else
					state_ = false;
			}

			DataCollection BooleanItemDisplay::getValues()
			{
				DataCollection d;
				d.add(desc()->tag(), QVariant(state_));

				return d;
			}

			void BooleanItemDisplay::changeEvent(QEvent* ev)
			{
				if (ev->type() == QEvent::FontChange)
				{
					QFontMetrics fm(font());
					int width = fm.horizontalAdvance(desc()->display()) + 8;
					setMinimumSize(QSize(width, 80));
				}
			}

			void BooleanItemDisplay::paintEvent(QPaintEvent* ev)
			{
				QPainter p(this);

				QFontMetrics fm(font());
				QRectF r(0, 0, width(), fm.height());

				QTextOption o;
				o.setAlignment(Qt::AlignCenter);
				p.drawText(r, desc()->display(), o);

				r = QRectF(0, fm.height() + 2, width(), height() - fm.height() - 2);

				QBrush b(state_ ? QColor(0, 255, 0, 255) : QColor(255, 0, 0, 255));
				p.setBrush(b);

				QPen pen(QColor(0, 0, 0, 255));
				pen.setWidth(3);
				p.setPen(pen);
				p.drawRect(r);
			}

			void BooleanItemDisplay::mousePressEvent(QMouseEvent* ev)
			{
				state_ = !state_;
				update();
			}
		}
	}
}
