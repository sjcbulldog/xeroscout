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
			AllianceGraphView::AllianceGraphView(QWidget* parent) : GraphPerTeamView(parent)
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

				msg_ = new QLabel(this);
				msg_->setText("Select teams to see graphs");
				hlay->addWidget(msg_);

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
					msg_->setText("");
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

