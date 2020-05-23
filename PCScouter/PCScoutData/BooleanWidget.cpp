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

#include "BooleanWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QEvent>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			BooleanWidget::BooleanWidget(const QString& label, QWidget* parent) : QWidget(parent)
			{
				QFontMetrics fm(font());
				int width = fm.horizontalAdvance(label);
				setMinimumSize(QSize(width + 8, 80));

				label_ = label;

				state_ = false;
			}

			BooleanWidget::~BooleanWidget()
			{
			}

			void BooleanWidget::changeEvent(QEvent* ev)
			{
				if (ev->type() == QEvent::FontChange)
				{
					QFontMetrics fm(font());
					int width = fm.horizontalAdvance(label_) + 8;
					setMinimumSize(QSize(width, 80));
				}
			}

			void BooleanWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter p(this);

				QFontMetrics fm(font());
				QRectF r(0, 0, width(), fm.height());

				QTextOption o;
				o.setAlignment(Qt::AlignCenter);
				p.drawText(r, label_, o);

				r = QRectF(0, fm.height() + 2, width(), height() - fm.height() - 2);

				QBrush b(state_ ? QColor(0, 255, 0, 255) : QColor(255, 0, 0, 255));
				p.setBrush(b);

				QPen pen(QColor(0, 0, 0, 255));
				pen.setWidth(3);
				p.setPen(pen);
				p.drawRect(r);
			}

			void BooleanWidget::mousePressEvent(QMouseEvent* ev)
			{
				state_ = !state_;
				update();
			}
		}
	}
}
