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

#include "AllianceGraphView.h"
#include "SelectTeamsDialog.h"
#include <QPushButton>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			AllianceGraphView::AllianceGraphView(QWidget* parent) : GraphView(parent)
			{
			}

			AllianceGraphView::~AllianceGraphView()
			{
			}

			void AllianceGraphView::createTop(QGroupBox *group)
			{
				QHBoxLayout* hlay = new QHBoxLayout();
				group->setLayout(hlay);

				teams_ = new QLabel(this);
				hlay->addWidget(teams_);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
				teams_->setSizePolicy(p);

				QPushButton* button = new QPushButton("Select Teams", this);
				hlay->addWidget(button);
				connect(button, &QPushButton::pressed, this, &AllianceGraphView::selectTeams);

				group->setTitle("Alliance Selection");

				GraphDescriptor desc(AllianceGraphView::Name);
				setDescriptor(desc);
			}

			void AllianceGraphView::selectTeams()
			{
				SelectTeamsDialog dialog(dataModel(), getTeams());
				if (dialog.exec() == QDialog::Accepted)
				{
					setTeams(dialog.keys());
					refreshCharts();
				}
			}

			void AllianceGraphView::updateTeamsLabel()
			{
				QString txt = "Teams: ";
				for (int i = 0; i < getTeams().size(); i++)
				{
					QString key = getTeams().at(i);
					if (key.startsWith("frc"))
						key = key.mid(3);

					if (i != 0)
						txt += ", ";
					txt += key;
				}
				teams_->setText(txt);
			}
		}
	}
}

