#pragma once

#include "scoutviews_global.h"
#include "GraphView.h"
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT PreMatchGraphView : public GraphView
			{
			public:
				PreMatchGraphView(QWidget* parent);
				virtual ~PreMatchGraphView();

				virtual void refreshView() {
					if (!changeTriggered())
					{
						updateComboBox();
						refreshCharts();
					}
				}

				virtual void clearView() {
				}

				static constexpr const char* Name = "match";

			protected:
				void createTop(QGroupBox* top);

			private:
				void matchChanged();
				void updateComboBox();

			private:
				QComboBox* box_;
				QString current_match_;
				QStringList team_keys_;
			};
		}
	}
}

