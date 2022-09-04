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
			class ChartViewWrapper : public QChartView
			{
				Q_OBJECT

			public:
				ChartViewWrapper(QWidget* parent, std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				virtual ~ChartViewWrapper();

				void mouseHovered(bool status, int index, QBarSet* set);
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

