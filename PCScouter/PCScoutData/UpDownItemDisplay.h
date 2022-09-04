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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QIntValidator>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class UpDownItemDisplay : public FormItemDisplay
			{
			public:
				UpDownItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent);
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
