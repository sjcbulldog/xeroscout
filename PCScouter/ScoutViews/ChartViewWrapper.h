#pragma once

#include <QChartView>
#include <QBarSet>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ChartViewWrapper : public QtCharts::QChartView
			{
			public:
				ChartViewWrapper(QWidget* parent);
				virtual ~ChartViewWrapper();

				void mouseHovered(bool status, int index, QtCharts::QBarSet* set);

			protected:
				void mouseMoveEvent(QMouseEvent* ev);

			private:
				QPoint pt_;
			};
		}
	}
}

