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
#include <qwidget.h>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class StartStopWidget : public QWidget
			{
				Q_OBJECT

			public:
				StartStopWidget(QWidget* parent);
				virtual ~StartStopWidget();

				void setStopped();
				bool state() {
					return state_;
				}

			signals:
				void cycleStarted();
				void cycleCompleted();
				void cycleAbandoned();

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;

			private:
				void drawRectWithText(QPainter& p, const QRect& r, const QString& txt, QColor rcolor, QColor tcolor);

			private:
				bool state_;
			};
		}
	}
}

