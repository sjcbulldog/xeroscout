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

#include "FormItem.h"
#include <QCheckBox>
#include "BooleanWidget.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class BooleanFormItem : public FormItem
			{
			public:
				BooleanFormItem(const QString& display, const QString& tag) : FormItem(display, tag)
				{
				}

				virtual ~BooleanFormItem()
				{
				}

				QVariant random(GameRandomProfile &profile) const override {
					return profile.generateRandomBool(tag());
				}

				QVariant::Type dataType() const override {
					return QVariant::Type::Bool;
				}

				QWidget* createWidget(const QString& name, QWidget* parent) const override {
					auto it = boxes_.find(name);
					assert(it == boxes_.end());

					BooleanWidget* box = new BooleanWidget(display(), parent);
					box->setChecked(false);

					//
					// HACK: fix this with a data storage model for scouting form items
					//
					BooleanFormItem* here = const_cast<BooleanFormItem*>(this);
					here->boxes_.insert(std::make_pair(name, box));
					return box;
				}

				void destroyWidget(const QString& name) const {

					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					delete it->second;

					//
					// HACK: fix this with a data storage model for scouting form items
					//
					BooleanFormItem* here = const_cast<BooleanFormItem*>(this);
					here->boxes_.erase(it);
				}

				QVariant getValue(const QString& name) const override {
					QVariant v;

					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					if (it->second->isChecked())
						v = QVariant(true);
					else
						v = QVariant(false);

					return v;
				}

				void setValue(const QString& name, const QVariant& v) const override {
					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					if (v.type() == QVariant::Type::Bool) {
						if (v.toBool())
							it->second->setChecked(true);
						else
							it->second->setChecked(false);
					}
				}

			private:
				std::map<QString, BooleanWidget*> boxes_;
			};

		}
	}
}
