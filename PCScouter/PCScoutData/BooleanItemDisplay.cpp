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
				setToolTip(desc->tag());
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
