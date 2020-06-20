#pragma once

#include "scoutviews_global.h"
#include "MatchTeamSelector.h"
#include "ViewBase.h"
#include "ZebraSequence.h"
#include "SequenceAnalyzer.h"
#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QTextBrowser>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraAnalysisView : public QWidget, public ViewBase
			{
			public:
				ZebraAnalysisView(QWidget* parent);
				~ZebraAnalysisView();

				void refreshView();
				void clearView();

				void madeActive() override;

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) {
					selector_->setDataModel(dm);
					ViewBase::setDataModel(dm);
				}

			protected:

			private:
				void regenerate();

				void generateRobot();
				void generateMatch();

				void printRawData(QString& html);
				void printSequences(QString& html);

				void matchesRobotsSelected();
				void comboxChanged(const QString& key);

			private:
				MatchTeamSelector* selector_;
				QGroupBox* report_;
				QTextBrowser* report_txt_;
				QString current_team_;
				std::list<std::shared_ptr<xero::scouting::datamodel::ZebraSequence>> sequences_;
				xero::scouting::datamodel::SequenceAnalyzer analyzer_;
				double idle_;
			};
		}
	}
}

