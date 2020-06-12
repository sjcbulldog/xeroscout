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

#include "TimeBoundWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <cmath>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			TimeBoundWidget::TimeBoundWidget(double minv, double maxv, QWidget* parent) : QWidget(parent)
			{
				minv_ = minv;
				maxv_ = maxv;
				range_start_ = minv_;
				range_end_ = maxv_;

				QFontMetrics fm(font());
				NumberHeight = fm.height() + 8;

				setMinimumHeight(NumberHeight + IndicatorHeight + TickRegionHeight);
				setMaximumHeight(NumberHeight + IndicatorHeight + TickRegionHeight);

				state_ = State::None;
				range_mode_ = true;

				current_time_ = std::numeric_limits<double>::max();
			}

			TimeBoundWidget::~TimeBoundWidget()
			{
			}

			void TimeBoundWidget::contextMenuEvent(QContextMenuEvent* ev)
			{
				QAction *act;

				QMenu menu(this);

				if (range_mode_)
				{
					act = menu.addAction("Autonomous");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::autonomous);

					act = menu.addAction("Teleop");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::teleop);

					act = menu.addAction("Endgame");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::endgame);
				}
				else
				{
					act = menu.addAction("Stop");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::animateStop);

					act = menu.addAction("1X");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::animate1X);

					act = menu.addAction("2X");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::animate2X);

					act = menu.addAction("4X");
					connect(act, &QAction::triggered, this, &TimeBoundWidget::animate4X);
				}

				menu.exec(ev->globalPos());
			}

			void TimeBoundWidget::animateStop()
			{
				emit changeAnimationState(false, 0.0);
			}

			void TimeBoundWidget::animate1X()
			{
				emit changeAnimationState(true, 1.0);
			}

			void TimeBoundWidget::animate2X()
			{
				emit changeAnimationState(true, 2.0);
			}

			void TimeBoundWidget::animate4X()
			{
				emit changeAnimationState(true, 4.0);
			}

			void TimeBoundWidget::autonomous()
			{
				range_start_ = minv_;
				range_end_ = 15.0;
				update();
				emit rangeChanged(range_start_, range_end_);
			}

			void TimeBoundWidget::teleop()
			{
				range_start_ = 15.0;
				range_end_ = 120.0;
				update();
				emit rangeChanged(range_start_, range_end_);
			}

			void TimeBoundWidget::endgame()
			{
				range_start_ = 120.0;
				range_end_ = maxv_;
				update();
				emit rangeChanged(range_start_, range_end_);
			}

			void TimeBoundWidget::mousePressEvent(QMouseEvent* ev)
			{
				int range = 12;

				if (ev->localPos().y() > IndicatorHeight)
					return;

				int rs = timeToPixel(range_start_);
				int re = timeToPixel(range_end_);

				if (std::abs(ev->localPos().x() - rs) < range)
				{
					state_ = State::DraggingStart;
				}
				else if (std::abs(ev->localPos().x() - re) < range)
				{
					state_ = State::DraggingEnd;
				}
			}

			void TimeBoundWidget::mouseReleaseEvent(QMouseEvent* ev)
			{
				state_ = State::None;
			}

			void TimeBoundWidget::mouseMoveEvent(QMouseEvent* ev)
			{
				double v = pixelToTime(ev->localPos().x());

				switch (state_)
				{
				case State::DraggingEnd:
					if (v < range_start_)
						v = range_start_ + 0.1;

					if (v < minv_)
						v = minv_;
					else if (v > maxv_)
						v = maxv_;

					range_end_ = v;
					update();


					emit rangeChanged(range_start_, range_end_);
					break;

				case State::DraggingStart:
					if (v > range_end_)
						v = range_end_ - 0.1;
					if (v < minv_)
						v = minv_;
					else if (v > maxv_)
						v = maxv_;

					range_start_ = v;
					update();

					emit rangeChanged(range_start_, range_end_);

					break;
				}

			}

			void TimeBoundWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter p(this);

				drawNumbers(p);
				drawIndicators(p);
				drawTickRegionHeight(p);
				drawCurrent(p);
			}

			void TimeBoundWidget::drawCurrent(QPainter& p)
			{
				if (!range_mode_ && current_time_ >= minv_ && current_time_ <= maxv_)
				{
					int height = 0;

					p.save();

					int pc = timeToPixel(current_time_);

					QBrush brush(QColor(0, 255, 0));
					p.setBrush(brush);
					p.drawRect(pc - IndicatorHeight / 2, height, IndicatorHeight, IndicatorHeight);
					p.restore();
				}
			}

			void TimeBoundWidget::drawNumbers(QPainter& p)
			{
				int height = TickRegionHeight + IndicatorHeight;

				p.save();

				QFontMetrics fm(p.font());

				QBrush brush(QColor(0, 0, 0));
				p.setBrush(brush);

				double start = std::round(minv_ / 10.0) * 10.0;
				double end = std::round(maxv_ / 10.0) * 10.0;

				double value = start;
				while (value <= end)
				{
					int x = (value - minv_) / (maxv_ - minv_) * width();
					QString txt = QString::number(value, 'f', 1);

					int swidth = fm.horizontalAdvance(txt);
					x = x - swidth / 2;

					if (x < 0)
						x = 0;
					else if (x + swidth > width())
						x = width() - swidth;

					QPoint pt(x, height + fm.height() + 4);

					p.drawText(pt, txt);

					value += 10.0;
				}

				p.restore();
			}

			void TimeBoundWidget::drawIndicators(QPainter& p)
			{
				int height = 0;
				QPoint pts[3];

				p.save();

				int rs = timeToPixel(range_start_);
				int re = timeToPixel(range_end_);

				QBrush fill(QColor(128, 128, 128));
				p.setBrush(fill);
				p.drawRect(rs, height + 4, re - rs, IndicatorHeight - 8);

				QBrush brush(QColor(0, 0, 255));
				p.setBrush(brush);

				pts[0].setX(rs);
				pts[0].setY(height + IndicatorHeight);
				pts[1].setX(rs - IndicatorHeight / 2);
				pts[1].setY(height);
				pts[2].setX(rs + IndicatorHeight / 2);
				pts[2].setY(height);
				p.drawPolygon(pts, 3);

				pts[0].setX(re);
				pts[0].setY(height + IndicatorHeight);
				pts[1].setX(re - IndicatorHeight / 2);
				pts[1].setY(height);
				pts[2].setX(re + IndicatorHeight / 2);
				pts[2].setY(height);
				p.drawPolygon(pts, 3);

				p.restore();
			}

			void TimeBoundWidget::drawTickRegionHeight(QPainter& p)
			{
				int height = NumberHeight;

				p.save();

				QPen pen(QColor(0, 0, 0));
				p.setPen(pen);

				double start = std::round(minv_ / 10.0) * 10.0;
				double end = std::round(maxv_ / 10.0) * 10.0;

				double value = start;
				while (value <= end)
				{
					int x = timeToPixel(value);
					if (x == 0)
						x = 1;

					p.drawLine(x, height, x, height + TickRegionHeight);

					int cnt = 0;
					for (double vsmall = value + 1.0; vsmall <= value + 9.0; vsmall += 1.0)
					{
						cnt++;
						int x = timeToPixel(vsmall);
						
						if (cnt == 5)
						{
							p.drawLine(x, height, x, height + 3 * TickRegionHeight / 4);
						}
						else
						{
							p.drawLine(x, height, x, height + TickRegionHeight / 2);
						}
					}

					value += 10.0;
				}

				p.restore();
			}
		}
	}
}

