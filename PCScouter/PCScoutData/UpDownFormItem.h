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
#include "UpDownItemDisplay.h"
#include <QtWidgets/QSpinBox>
#include <QtCore/QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class UpDownFormItem : public FormItemDesc
			{
			public:
				UpDownFormItem(const QString& display, const QString& tag, int minv, int maxv) : FormItemDesc(display, tag)
				{
					minv_ = minv;
					maxv_ = maxv;
					addField(std::make_shared<FieldDesc>(tag, FieldDesc::Type::Integer, true));
				}

				UpDownFormItem(const QString& display, const QString& tag, int minv, int maxv, int minlimit, int maxlimit) : FormItemDesc(display, tag)
				{
					minv_ = minv;
					maxv_ = maxv;
					auto field = std::make_shared<FieldDesc>(tag, FieldDesc::Type::Integer, true);
					field->setLimits(minlimit, maxlimit);
					addField(field);
				}

				virtual ~UpDownFormItem()
				{
				}

				int minValue() const {
					return minv_;
				}

				int maxValue() const {
					return maxv_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new UpDownItemDisplay(images, this, parent);
				}

			private:
				int minv_;
				int maxv_;
			};

		}
	}
}
