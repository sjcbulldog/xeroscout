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
#include <QCheckBox>
#include "BooleanItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class BooleanFormItem : public FormItemDesc
			{
			public:
				BooleanFormItem(const QString& display, const QString& tag) : FormItemDesc(display, tag)
				{
					addField(std::make_pair(tag, QVariant::Bool));
				}

				virtual ~BooleanFormItem()
				{
				}

				virtual DataCollection random(GameRandomProfile& profile) const
				{
					DataCollection d;
					QVariant v = profile.generateRandomBool(tag());
					d.add(tag(), v);
					return d;
				}

				virtual FormItemDisplay* createDisplay(QWidget *parent) const
				{
					return new BooleanItemDisplay(this, parent);
				}
			};
		}
	}
}
