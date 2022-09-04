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

#include "FormItemDesc.h"
#include "ChoiceItemDisplay.h"
#include <QtCore/QStringList>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ChoiceFormItem : public FormItemDesc
			{
			public:
				ChoiceFormItem(const QString& display, const QString& tag, const QStringList& choices) : FormItemDesc(display, tag) 
				{
					choices_ = choices;
					addField(std::make_shared<FieldDesc>(tag, choices, true));
				}

				virtual ~ChoiceFormItem() {
				}

				const QStringList& choices() const {
					return choices_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new ChoiceItemDisplay(images, this, parent);
				}

			private:
				QStringList choices_;
			};

		}
	}
}
