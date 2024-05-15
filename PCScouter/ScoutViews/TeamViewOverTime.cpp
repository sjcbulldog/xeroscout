#include "TeamViewOverTime.h"
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

using namespace xero::scouting::datamodel;
using namespace xero::expr;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			TeamViewOverTime::TeamViewOverTime(QWidget* parent) : GraphPerMatchView(parent)
			{
				box_ = nullptr;
			}

			TeamViewOverTime::~TeamViewOverTime()
			{
			}

			void TeamViewOverTime::createTop(QGroupBox* group)
			{
				QHBoxLayout* hlay = new QHBoxLayout();
				group->setLayout(hlay);
				box_ = new QComboBox(group);
				hlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &TeamViewOverTime::teamChanged);

				group->setTitle("Team View Over Matches");

				GraphDescriptor desc(TeamViewOverTime::Name);
				setDescriptor(desc);
			}

			void TeamViewOverTime::updateComboBox()
			{
				box_->blockSignals(true);

				box_->clear();
				auto dm = dataModel();

				if (dm != nullptr)
				{
					for (auto team : dm->teams()) {
						box_->addItem(QString::number(team->number()) + ":" + team->nick(), team->key());
					}

					box_->blockSignals(false);
				}

				if (box_->count() > 0)
				{
					box_->setCurrentIndex(0);
					teamChanged();
				}
			}

			void TeamViewOverTime::teamChanged()
			{
				if (box_->currentIndex() != -1)
				{
					QString key = box_->currentData().toString();
					setTeam(key);
					refreshCharts();
				}
			}
		}
	}
}

