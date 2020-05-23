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

