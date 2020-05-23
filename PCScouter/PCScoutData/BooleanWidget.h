//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class BooleanWidget : public QWidget
			{
			public:
				BooleanWidget(const QString& label, QWidget* parent = nullptr);
				virtual ~BooleanWidget();

				void setChecked(bool b) {
					state_ = b;
					update();
				}

				bool isChecked() const {
					return state_;
				}

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void changeEvent(QEvent* ev) override;

			private:
				QString label_;
				bool state_;
			};

		}
	}
}
