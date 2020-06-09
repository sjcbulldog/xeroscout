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
#include "TimerCountItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class TimerCountFormItem : public FormItemDesc
			{
			public:
				constexpr static const char* CompletedTimeField = "completedtime";
				constexpr static const char* CompletedCountField = "completedcount";
				constexpr static const char* AbandonedTimeField =  "abandonedtime";
				constexpr static const char* AbandonedCountField = "abandonedcount";

			public:
				TimerCountFormItem(const QString& display, const QString& tag, int minv, int maxv) : FormItemDesc(display, tag)
				{
					addField(std::make_shared<FieldDesc>(genComplexName(tag, CompletedTimeField), FieldDesc::Type::Double, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, CompletedCountField), FieldDesc::Type::Integer, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, AbandonedTimeField), FieldDesc::Type::Double, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, AbandonedCountField), FieldDesc::Type::Integer, true));
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~TimerCountFormItem()
				{
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new TimerCountItemDisplay(images, this, parent);
				}

			private:
				int minv_;
				int maxv_;
			};
		}
	}
}

