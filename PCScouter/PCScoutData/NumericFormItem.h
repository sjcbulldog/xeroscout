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
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class NumericFormItem : public FormItem
			{
			public:
				NumericFormItem(const QString& display, const QString& tag, int minv, int maxv) : FormItem(display, tag)
				{
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~NumericFormItem()
				{
				}

				QVariant random(GameRandomProfile &profile) const override {
					return profile.generateRandomNumeric(tag(), minv_, maxv_);
				}

				QVariant::Type dataType()  const override {
					return QVariant::Type::Int;
				}

				QWidget* createWidget(const QString& name, QWidget* parent) const override {
					auto it = editors_.find(name);
					assert(it == editors_.end());

					QWidget* w = new QWidget();
					QHBoxLayout* layout = new QHBoxLayout();
					w->setLayout(layout);

					QLabel* label = new QLabel(display(), w);
					layout->addWidget(label);
					QFont font = label->font();
					font.setPointSizeF(16.0);
					label->setFont(font);

					QLineEdit* edit = new QLineEdit(parent);
					edit->setValidator(new QIntValidator(minv_, maxv_, parent));
					layout->addWidget(edit);
					font = edit->font();
					font.setPointSizeF(16.0);
					edit->setFont(font);

					edit->setText(QString::number(minv_));

					//
					// HACK: fix this with a data storage model for scouting form items
					//
					NumericFormItem* here = const_cast<NumericFormItem*>(this);
					here->editors_.insert(std::make_pair(name, edit));

					return w;
				}

				void destroyWidget(const QString& name)  const override {

					auto it = editors_.find(name);
					assert(it != editors_.end());

					delete it->second;

					//
					// HACK: fix this with a data storage model for scouting form items
					//
					NumericFormItem* here = const_cast<NumericFormItem*>(this);
					here->editors_.erase(it);
				}

				QVariant getValue(const QString& name) const override {
					QVariant v;

					auto it = editors_.find(name);
					assert(it != editors_.end());

					int i = it->second->text().toInt();
					v = QVariant(i);

					return v;
				}

				void setValue(const QString& name, const QVariant& v) const override {

					auto it = editors_.find(name);
					assert(it != editors_.end());

					if (v.type() == QVariant::Type::Int) {
						QString txt = QString::number(v.toInt());
						it->second->setText(txt);
					}
				}

			private:
				int minv_;
				int maxv_;
				std::map<QString, QLineEdit*> editors_;
			};

		}
	}
}
