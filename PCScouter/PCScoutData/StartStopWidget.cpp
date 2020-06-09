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
