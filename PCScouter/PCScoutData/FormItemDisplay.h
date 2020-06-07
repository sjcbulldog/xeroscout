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

#include "ImageSupplier.h"
#include "DataCollection.h"
#include "FormItemDesc.h"
#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class FormItemDisplay : public QWidget
			{
			public:
				FormItemDisplay(ImageSupplier &images, const FormItemDesc *desc, QWidget* parent);
				virtual ~FormItemDisplay();

				virtual void setValues(const DataCollection& data) = 0;
				virtual DataCollection getValues() = 0;

				const FormItemDesc *desc() {
					return desc_;
				}

			protected:
				ImageSupplier& images() {
					return images_;
				}

			private:
				const FormItemDesc * desc_;
				ImageSupplier& images_;
			};
		}
	}
}
