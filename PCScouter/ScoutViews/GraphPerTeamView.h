#pragma once

#include "GraphView.h"
#include "GraphDescriptor.h"
#include "ScoutingDataMap.h"

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT GraphPerTeamView : public GraphView
			{
			public:
				GraphPerTeamView(QWidget* parent = nullptr) : GraphView(true, true, parent) {
				}

				virtual ~GraphPerTeamView() {
				}

			protected:
				void setTeamsColors(const QStringList& teams, const QStringList& aug) {
					team_keys_ = teams;
					augmentation_ = aug;
				}

				void setTeams(const QStringList& teams) {
					team_keys_ = teams;
					augmentation_.clear();
				}

				void setTeam(const QString& team) {
					team_keys_.clear();
					team_keys_.push_back(team);
					augmentation_.clear();
				}

				const QStringList& getTeams() const {
					return team_keys_;
				}

				bool generateOneChart(std::shared_ptr<const xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, 
									  std::shared_ptr<ChartViewWrapper> chart) override;

				bool getData(std::map<QString, std::vector<QVariant>>& data, const QStringList& exprlist);

			private:
				QStringList team_keys_;
				QStringList augmentation_;
			};
		}
	}
}
