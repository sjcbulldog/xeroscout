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

#include <QPushButton>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QIntValidator>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class UpDownWidget : public QWidget
			{
			public:
				UpDownWidget(const QString& label, QWidget* parent);
				virtual ~UpDownWidget();

				int value();

				void setValue(int v);

				int minimum() {
					return minv_;
				}

				void setMinimum(int v) {
					minv_ = v;
					if (value() < minv_)
						setValue(minv_);
					editor_->setValidator(new QIntValidator(minv_, maxv_, this));
				}

				int maximum() {
					return maxv_;
				}

				void setMaximum(int v) {
					maxv_ = v;
					if (value() > maxv_)
						setValue(maxv_);
					editor_->setValidator(new QIntValidator(minv_, maxv_, this));
				}

			protected:
				void resizeEvent(QResizeEvent* ev) override;
				void changeEvent(QEvent* ev) override;

			private:
				void doLayout();
				void upButtonPressed();
				void downButtonPressed();

			public:
				QLineEdit* editor_;
				QPushButton* up_;
				QPushButton* down_;
				QLabel* label_;
				int minv_;
				int maxv_;
			};

		}
	}
}
