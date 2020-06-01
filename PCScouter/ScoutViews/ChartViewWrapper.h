#pragma once

#include "GraphDescriptor.h"
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
				ChartViewWrapper(QWidget* parent, std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				virtual ~ChartViewWrapper();

				void mouseHovered(bool status, int index, QtCharts::QBarSet* set);
				void editTitle();

			signals:
				void titleChanged(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& text);

			protected:
				void mouseMoveEvent(QMouseEvent* ev);
				void mouseDoubleClickEvent(QMouseEvent* ev);

			private:
				void editorDone();
				void editorAborted();


			private:
				QPoint pt_;
				EditName* editor_;
				std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane_;
			};
		}
	}
}

