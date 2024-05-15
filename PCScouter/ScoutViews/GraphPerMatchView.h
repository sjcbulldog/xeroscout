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
			class SCOUTVIEWS_EXPORT GraphPerMatchView : public GraphView
			{
			public:
				GraphPerMatchView(QWidget* parent = nullptr) : GraphView(false, true, parent) {
				}

				virtual ~GraphPerMatchView() {
				}

			protected:
				void setTeam(QString& teamkey) {
					team_ = teamkey;
				}

				bool getData(xero::scouting::datamodel::ScoutingDataSet& ds, const QStringList& exprlist);
				virtual bool generateOneChart(std::shared_ptr<const xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart);

			private:
				QString team_;
			};
		}
	}
}

