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

