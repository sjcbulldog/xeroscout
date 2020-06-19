#include "MatchTeamSelector.h"
#include <QBoxLayout>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			MatchTeamSelector::MatchTeamSelector(std::shared_ptr<ScoutingDataModel> dm, QWidget* parent) : QWidget(parent)
			{
				dm_ = dm;
				QHBoxLayout* hlay = new QHBoxLayout();
				setLayout(hlay);

				matches_ = new QRadioButton("Match", this);
				matches_->setChecked(true);
				hlay->addWidget(matches_);
				(void)connect(matches_, &QRadioButton::toggled, this, &MatchTeamSelector::matchSelectedCB);

				robots_ = new QRadioButton("Robot", this);
				hlay->addWidget(robots_);
				(void)connect(robots_, &QRadioButton::toggled, this, &MatchTeamSelector::robotSelectedCB);

				box_ = new QComboBox(this);
				hlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &MatchTeamSelector::comboxChangedCB);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
				box_->setSizePolicy(p);

				update();
			}

			MatchTeamSelector::~MatchTeamSelector()
			{
			}

			void MatchTeamSelector::update()
			{
				box_->clear();

				if (dm_ == nullptr)
					return;

				if (matches_->isChecked())
				{
					box_->blockSignals(true);
					box_->clear();

					for (auto m : dm_->matches())
					{
						box_->addItem(m->title(), m->key());
					}
					box_->blockSignals(false);
					box_->setCurrentIndex(0);
				}
				else
				{
					box_->blockSignals(true);
					box_->clear();
					std::list<std::shared_ptr<const DataModelTeam>> teams = dm_->teams();
					teams.sort([](std::shared_ptr<const DataModelTeam> a, std::shared_ptr<const DataModelTeam> b) -> bool
						{
							return a->number() < b->number();
						});

					for (auto t : teams)
						box_->addItem(QString::number(t->number()) + " - " + t->nick(), t->key());
					box_->blockSignals(false);
					box_->setCurrentIndex(0);
				}
			}

			void MatchTeamSelector::robotSelectedCB(bool checked)
			{
				if (checked)
				{
					blockSignals(true);
					update();
					blockSignals(false);

					emit robotSelected();
				}
			}
			
			void MatchTeamSelector::matchSelectedCB(bool checked)
			{
				if (checked)
				{
					blockSignals(true);
					update();
					blockSignals(false);

					emit matchSelected();
				}
			}

			void MatchTeamSelector::comboxChangedCB(int index)
			{
				QString item = box_->currentData().toString();
				emit selectedItemChanged(item);
			}
		}
	}
}
