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
#include "UpDownWidget.h"
#include <QSpinBox>
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class UpDownFormItem : public FormItem
			{
			public:
				UpDownFormItem(const QString& display, const QString& tag, int minv, int maxv) : FormItem(display, tag)
				{
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~UpDownFormItem()
				{
				}

				QVariant random(GameRandomProfile& profile) const override {
					return profile.generateRandomNumeric(tag(), minv_, maxv_);
				}

				QVariant::Type dataType()  const override {
					return QVariant::Type::Int;
				}


				QWidget* createWidget(const QString& name, QWidget* parent) const override {
					auto it = boxes_.find(name);
					assert(it == boxes_.end());

					UpDownWidget* w = new UpDownWidget(display(), parent);
					w->setMinimum(minv_);
					w->setMaximum(maxv_);

					//
					// HACK: fix this with a model for form item storage
					//
					UpDownFormItem* here = const_cast<UpDownFormItem*>(this);
					here->boxes_.insert(std::make_pair(name, w));
					w->setValue(minv_);

					return w;
				}

				void destroyWidget(const QString& name) const override {

					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					delete it->second;

					//
					// HACK: fix this with a model for form item storage
					//
					UpDownFormItem* here = const_cast<UpDownFormItem*>(this);
					here->boxes_.erase(it);
				}

				QVariant getValue(const QString& name) const override {
					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					QVariant v;
					v = QVariant(it->second->value());

					return v;
				}

				void setValue(const QString& name, const QVariant& v) const override {
					auto it = boxes_.find(name);
					assert(it != boxes_.end());

					if (v.type() == QVariant::Type::Int) {
						it->second->setValue(v.toInt());
					}
				}

			private:
				int minv_;
				int maxv_;

#ifdef XERO_QT_WIDGETS_ONLY
				std::map<QString, QSpinBox*> boxes_;
#else
				std::map<QString, UpDownWidget*> boxes_;
#endif
			};

		}
	}
}
