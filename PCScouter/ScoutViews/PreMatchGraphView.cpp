//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "PreMatchGraphView.h"

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PreMatchGraphView::PreMatchGraphView(QWidget* parent) : GraphView(parent)
			{
				box_ = nullptr;

			}

			PreMatchGraphView::~PreMatchGraphView()
			{
			}

			void PreMatchGraphView::createTop(QGroupBox *group)
			{
				QHBoxLayout* hlay = new QHBoxLayout();
				group->setLayout(hlay);
				box_ = new QComboBox(group);
				hlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &PreMatchGraphView::matchChanged);

				group->setTitle("Pre Match Selection");

				GraphDescriptor desc(PreMatchGraphView::Name);
				setDescriptor(desc);
			}

			void PreMatchGraphView::updateComboBox()
			{
				box_->blockSignals(true);

				box_->clear();
				auto dm = dataModel();

				if (dm != nullptr)
				{
					for (auto match : dm->matches())
						box_->addItem(match->title(), QVariant(match->key()));

					box_->blockSignals(false);
				}

				if (box_->count() > 0)
				{
					box_->setCurrentIndex(0);
					matchChanged();
				}
			}

			void PreMatchGraphView::matchChanged()
			{
				QStringList teams;
				QStringList aug;

				if (box_->currentIndex() != -1)
				{
					QString key = box_->currentData().toString();
					auto m = dataModel()->findMatchByKey(key);

					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString team = m->team(c, slot);
						teams.push_back(team);
						aug.push_back("red");
					}

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString team = m->team(c, slot);
						teams.push_back(team);
						aug.push_back("blue");
					}
				}

				setTeamsColors(teams, aug);
				refreshCharts();
			}
		}
	}
}
