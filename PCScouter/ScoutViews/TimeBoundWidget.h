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

				void autonomous();
				void teleop();
				void endgame();

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
					None,
				};

			private:
				double minv_;
				double maxv_;

				double range_start_;
				double range_end_;

				int NumberHeight;
				State state_;
			};
		}
	}
}
