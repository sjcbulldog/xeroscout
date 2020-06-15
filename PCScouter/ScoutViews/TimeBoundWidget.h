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

#pragma once

#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class TimeBoundWidget : public QWidget
			{
				Q_OBJECT

			public:
				TimeBoundWidget(double min, double max, QWidget* parent);
				virtual ~TimeBoundWidget();

				void setCurrentTime(double t) {
					current_time_ = t;
				}

				double currentTime() const {
					return current_time_;
				}

				void setRangeMode(bool b) {
					range_mode_ = b;
					current_time_ = range_start_;
					update();
				}

				bool rangeMode() const {
					return range_mode_;
				}

				double minimum() const {
					return minv_;
				}

				void setMinimum(double v) {
					minv_ = v;

					if (range_start_ < minv_ || range_end_ < minv_)
					{
						range_start_ = minv_;
						range_end_ = maxv_;
					}
				}

				double maximum() const {
					return maxv_;
				}

				void setMaximum(double v) {
					maxv_ = v;

					if (range_start_ > maxv_ || range_end_ > maxv_)
					{
						range_start_ = minv_;
						range_end_ = maxv_;
					}
				}

				double rangeStart() const {
					return range_start_;
				}

				void setRangeStart(double v) {
					if (v >= minv_ && v <= maxv_ && v <= range_end_)
						range_start_ = v;
				}

				double rangeEnd() const {
					return range_end_;
				}

				void setRangeEnd(double v) {
					if (v >= minv_ && v <= maxv_ && v >= range_start_)
						range_end_ = v;
				}

			signals:
				void rangeChanged(double rangemin, double rangemax);
				void changeAnimationState(bool state, double multi);
				void currentTimeChanged(double now);

			protected:
				void paintEvent(QPaintEvent* ev) override ;
				void mousePressEvent(QMouseEvent* ev) override ;
				void mouseReleaseEvent(QMouseEvent* ev) override;
				void mouseMoveEvent(QMouseEvent* ev) override ;
				void contextMenuEvent(QContextMenuEvent* ev) override;

			private:
				void drawNumbers(QPainter& p);
				void drawIndicators(QPainter &p);
				void drawTickRegionHeight(QPainter &p);
				void drawCurrent(QPainter& p);

				void autonomous();
				void teleop();
				void endgame();
				void completeMatch();

				void animateStop();
				void animate1X();
				void animate2X();
				void animate4X();

				int timeToPixel(double t) {
					return static_cast<int>((t - minv_) / (maxv_ - minv_) * width());
				}

				double pixelToTime(int p) {
					return (double)p / (double)width() * (maxv_ - minv_) + minv_;
				}

			private:
				static constexpr const int IndicatorHeight = 12;
				static constexpr const int TickRegionHeight = 12;

				enum class State
				{
					DraggingStart,
					DraggingEnd,
					DraggingCurrent,
					None,
				};

			private:
				double minv_;
				double maxv_;

				double range_start_;
				double range_end_;
				double current_time_;

				int NumberHeight;
				State state_;

				bool range_mode_;
			};
		}
	}
}
