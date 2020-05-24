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

#include "FormView.h"
#include "FlowLayout.h"
#include <QBoxLayout>
#include <QSpinBox>
#include <cassert>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			FormView::FormView(QString name, QString title, QColor titlec, QWidget* parent) : QWidget(parent)
			{
				name_ = name;
				title_txt_ = title;
				instance_ = nullptr;

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				titles_ = new QLabel(title, this);
				titles_->setAlignment(Qt::AlignCenter);
				QPalette pal = titles_->palette();
				pal.setColor(titles_->foregroundRole(), titlec);
				titles_->setPalette(pal);

				lay->addWidget(titles_);

				QFont f = titles_->font();
				f.setUnderline(true);
				f.setBold(true);
				f.setPointSizeF(16.0);
				titles_->setFont(f);

				tabs_ = new QTabWidget(this);
				lay->addWidget(tabs_);

				f = tabs_->font();
				f.setPointSizeF(18.0);
				tabs_->setFont(f);
			}

			FormView::~FormView()
			{
			}

			void FormView::createSection(std::shared_ptr<const FormSection> section)
			{
				QWidget* tab = new QWidget(this);
				FlowLayout* layout = new FlowLayout();
				tab->setLayout(layout);

				for (auto item : section->items()) {
					FormItemDisplay* w = item->createDisplay(this);
					layout->addWidget(w);
					instance_->addDisplayItem(item->tag(), w);
				}

				tabs_->addTab(tab, section->name());
			}

			void FormView::setScoutingForm(std::shared_ptr<const ScoutingForm> form)
			{
				clearView();

				form_ = form;
				instance_ = std::make_shared<FormInstance>(form);
				auto& sections = form_->sections();
				for (auto section : sections) {
					createSection(section);
				}
			}

			void FormView::clearView()
			{
				instance_->clear();
				for (int i = 0; i < tabs_->count(); i++) {
					delete tabs_->widget(i);
				}
				tabs_->clear();
			}

			void FormView::extractData(ScoutingDataMapPtr ptr)
			{
				instance_->extract(ptr);
			}

			void FormView::assignData(ConstScoutingDataMapPtr ptr)
			{
				instance_->assign(ptr);
			}
		}
	}
}
