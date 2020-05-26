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

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class BooleanItemDisplay : public FormItemDisplay
			{
			public:
				BooleanItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent = nullptr);
				virtual ~BooleanItemDisplay();

				virtual void setValues(const DataCollection& data) ;
				virtual DataCollection getValues() ;

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void changeEvent(QEvent* ev) override;

			private:
				bool state_;
			};

		}
	}
}
