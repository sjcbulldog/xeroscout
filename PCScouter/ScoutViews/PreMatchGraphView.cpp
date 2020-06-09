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
