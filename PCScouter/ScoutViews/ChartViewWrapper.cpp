#include "ChartViewWrapper.h"
#include <QBarSet>
#include <QDebug>
#include <QToolTip>

using namespace QtCharts;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ChartViewWrapper::ChartViewWrapper(QWidget* parent) : QChartView(parent)
			{
			}

			ChartViewWrapper::~ChartViewWrapper()
			{
			}

			void ChartViewWrapper::mouseMoveEvent(QMouseEvent* ev)
			{
				QChartView::mouseMoveEvent(ev);
				pt_ = ev->globalPos();
			}

			void ChartViewWrapper::mouseHovered(bool status, int index, QBarSet* set)
			{
				if (status)
				{
					QString txt = QString::number(set->at(index));
					QToolTip::showText(pt_, txt);
				}
				else
				{
					QToolTip::hideText();
				}
			}
		}
	}
}
