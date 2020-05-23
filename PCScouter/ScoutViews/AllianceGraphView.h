#pragma once

#include "scoutviews_global.h"
#include "GraphView.h"
#include <QLabel>
#include <QGroupBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT AllianceGraphView : public GraphView
			{
			public:
				AllianceGraphView(QWidget* parent);
				virtual ~AllianceGraphView();

				virtual void refreshView() {
					refreshCharts();
				}

				virtual void clearView() {
					clearPanes();
				}

				static constexpr const char* Name = "alliance";

			protected:
				void createTop(QGroupBox* top) override;

			private:
				void selectTeams();
				void updateTeamsLabel();

			private:
				QLabel* teams_;
			};
		}
	}
}

