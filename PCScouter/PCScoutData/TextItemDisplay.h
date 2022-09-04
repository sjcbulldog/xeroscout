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
#include "FormItemDisplay.h"
#include <QtWidgets/QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class TextItemDisplay : public FormItemDisplay
			{
			public:
				TextItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent);
				virtual ~TextItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			private:
				QLineEdit* edit_;
			};
		}
	}
}

