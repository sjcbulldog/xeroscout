#pragma once

#include "EditName.h"
#include <QChartView>
#include <QBarSet>
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ChartViewWrapper : public QtCharts::QChartView
			{
				Q_OBJECT

			public:
				ChartViewWrapper(QWidget* parent);
				virtual ~ChartViewWrapper();

				void mouseHovered(bool status, int index, QtCharts::QBarSet* set);
				void editTitle();

			signals:
				void titleChanged();

			protected:
				void mouseMoveEvent(QMouseEvent* ev);

			private:
				void editorDone();
				void editorAborted();


			private:
				QPoint pt_;
				EditName* editor_;
			};
		}
	}
}

