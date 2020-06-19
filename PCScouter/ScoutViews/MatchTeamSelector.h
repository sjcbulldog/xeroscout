#pragma once

#include "ScoutingDataModel.h"
#include <QWidget>
#include <QComboBox>
#include <QRadioButton>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class MatchTeamSelector : public QWidget
			{
				Q_OBJECT

			public:
				MatchTeamSelector(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, QWidget* parent);
				virtual ~MatchTeamSelector();

				void update();
				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) {
					dm_ = dm;
					update();
				}

				void setMatches() {
					matches_->setChecked(true);
				}

				void setRobots() {
					robots_->setChecked(true);
				}

				void clear() {
					box_->clear();
				}

				QString currentKey() {
					return box_->currentData().toString();
				}

				bool isMatchesSelected() const {
					return matches_->isChecked();
				}

				bool isRobotsSelected() const {
					return robots_->isChecked();
				}

			signals:
				void robotSelected();
				void matchSelected();
				void selectedItemChanged(const QString& item);

			private:
				void robotSelectedCB(bool checked);
				void matchSelectedCB(bool checked);
				void comboxChangedCB(int index);

			private:
				std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
				QComboBox* box_;
				QRadioButton* matches_;
				QRadioButton* robots_;
			};
		}
	}
}
