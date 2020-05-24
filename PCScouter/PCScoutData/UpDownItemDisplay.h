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

#include "FormItemDisplay.h"
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
			class UpDownItemDisplay : public FormItemDisplay
			{
			public:
				UpDownItemDisplay(const FormItemDesc *desc, QWidget* parent);
				virtual ~UpDownItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			protected:
				void resizeEvent(QResizeEvent* ev) override;
				void changeEvent(QEvent* ev) override;

			private:
				int value() const ;
				void doLayout();
				void upButtonPressed();
				void downButtonPressed();

			public:
				QLineEdit* editor_;
				QPushButton* up_;
				QPushButton* down_;
				QLabel* label_;
			};

		}
	}
}
