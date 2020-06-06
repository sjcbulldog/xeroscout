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

#include "FormItemDesc.h"
#include "UpDownItemDisplay.h"
#include <QSpinBox>
#include <QDebug>

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
