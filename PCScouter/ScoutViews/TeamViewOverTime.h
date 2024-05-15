#pragma once
#include "GraphPerMatchView.h"
#include "GraphDescriptor.h"
#include "ScoutDataExprContext.h"
#include "ScoutingDataMap.h"
#include "scoutviews_global.h"

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class TeamViewOverTime : public GraphPerMatchView
			{
			public:
				TeamViewOverTime(QWidget* parent);
				virtual ~TeamViewOverTime();

				virtual void refreshView() {
					if (!changeTriggered())
					{
						updateComboBox();
						refreshCharts();
					}
				}

				virtual void clearView() {
				}

				static constexpr const char* Name = "time";

			private:
				void updateComboBox();
				void createTop(QGroupBox* group);
				void teamChanged();

			private:
				QComboBox* box_;
				QString current_team_;
			};
		}
	}
}

