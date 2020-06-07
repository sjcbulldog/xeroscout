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

#include "StartStopWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			StartStopWidget::StartStopWidget(QWidget* parent) : QWidget(parent)
			{
				state_ = false;
				setMinimumSize(QSize(150, 200));
			}

			StartStopWidget::~StartStopWidget()
			{
			}

			void StartStopWidget::setStopped()
			{
				if (state_)
				{
					state_ = false;
				}

				update();
			}

			void StartStopWidget::mousePressEvent(QMouseEvent* ev)
			{
				if (state_)
				{
					QRect r(0, 0, width(), height() / 2);
					state_ = false;
					if (r.contains(ev->pos()))
						emit cycleCompleted() ;
					else
						emit cycleAbandoned() ;
				}
				else
				{
					state_ = true;
					emit cycleStarted();
				}
				update();
			}

			void StartStopWidget::drawRectWithText(QPainter &p, const QRect& r, const QString& txt, QColor rcolor, QColor tcolor)
			{
				QBrush br = QBrush(rcolor);
				p.setBrush(br);
				QFontMetrics fm(p.font());

				p.drawRect(r);

				QPen pen(tcolor);
				p.setPen(pen);
				int txtw = fm.horizontalAdvance(txt);
				QPoint pt(r.x() + r.width() / 2 - txtw / 2, r.y() + r.height() / 2);
				p.drawText(pt, txt);
			}

			void StartStopWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter p(this);

				if (!state_)
				{
					drawRectWithText(p, rect(), "Start", QColor(0, 255, 0), QColor(0, 0, 0));
				}
				else
				{
					QRect r(0, 0, width(), height() / 2);
					drawRectWithText(p, r, "Complete", QColor(255, 0, 0), QColor(255, 255, 255));

					r = QRect(0, height() / 2, width(), height() / 2);
					drawRectWithText(p, r, "Abandon", QColor(0, 0, 255), QColor(255, 255, 255));
				}
			}
		}
	}
}
