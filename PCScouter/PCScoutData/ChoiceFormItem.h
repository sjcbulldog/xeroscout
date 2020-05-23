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
#include <QStringList>
#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class ChoiceFormItem : public FormItem
			{
			public:
				ChoiceFormItem(const QString& display, const QString& tag, const QStringList& choices) : FormItem(display, tag) {
					choices_ = choices;
				}

				virtual ~ChoiceFormItem() {
				}

				QVariant random(GameRandomProfile& profile) const override {
					return profile.generateRandomChoice(tag(), choices_);
				}

				QVariant::Type dataType()  const override {
					return QVariant::Type::String;
				}

				const QStringList& choices() {
					return choices_;
				}

				QWidget* createWidget(const QString& name, QWidget* parent) const override {
					auto it = combos_.find(name);
					assert(it == combos_.end());

					QWidget* w = new QWidget();
					QHBoxLayout* layout = new QHBoxLayout();
					w->setLayout(layout);

					QLabel* label = new QLabel(display(), w);
					layout->addWidget(label);
					QFont font = label->font();
					font.setPointSizeF(16.0);
					label->setFont(font);

					QComboBox* combo = new QComboBox(parent);

					// 
					// HACK: fix this with a data storage model for scouting forms
					//
					ChoiceFormItem* here = const_cast<ChoiceFormItem*>(this);
					here->combos_.insert(std::make_pair(name, combo));

					layout->addWidget(combo);
					for (const QString& choice : choices_)
						combo->addItem(choice);

					combo->setCurrentIndex(0);

					font = combo->font();
					font.setPointSizeF(16.0);
					combo->setFont(font);

					return w;
				}

				void destroyWidget(const QString& name)  const override {

					auto it = combos_.find(name);
					assert(it != combos_.end());

					delete it->second;

					// 
					// HACK: fix this with a data storage model for scouting forms
					//
					ChoiceFormItem* here = const_cast<ChoiceFormItem*>(this);
					here->combos_.erase(it);
				}

				QVariant getValue(const QString& name)  const override {
					QVariant v;

					auto i = combos_.find(name);
					assert(i != combos_.end());

					v = QVariant(i->second->currentText());

					return v;
				}

				void setValue(const QString& name, const QVariant& v)  const override {

					auto i = combos_.find(name);
					assert(i != combos_.end());

					if (v.type() == QVariant::Type::String) {
						int index = i->second->findText(v.toString(), Qt::MatchExactly);
						if (index != -1)
							i->second->setCurrentIndex(index);
					}
				}

			private:
				QStringList choices_;
				std::map<QString, QComboBox*> combos_;
			};

		}
	}
}
