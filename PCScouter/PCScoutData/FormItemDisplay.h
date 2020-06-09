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
				
				virtual void setScale(double scale) {
					scale_ = scale;
				}

				double scale() const {
					return scale_;
				}

			protected:
				ImageSupplier& images() {
					return images_;
				}

			private:
				const FormItemDesc * desc_;
				ImageSupplier& images_;
				double scale_;
			};
		}
	}
}
