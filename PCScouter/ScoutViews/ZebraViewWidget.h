#pragma once

#include "scoutviews_global.h"
#include "ViewBase.h"
#include "PathFieldView.h"
#include "DataModelMatch.h"
#include "TimeBoundWidget.h"
#include <QWidget>
#include <QComboBox>
#include <QRadioButton>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraViewWidget : public QWidget, public ViewBase
			{
			public:
				ZebraViewWidget(QWidget* parent);
				virtual ~ZebraViewWidget();

				void clearView();
				void refreshView();

				PathFieldView* field() {
					return field_;
				}

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					ViewBase::setDataModel(model);
					matchesSelected(true);
				}

			private:
				void matchesSelected(bool checked);
				void robotSelected(bool checked);
				void comboxChanged(int index);

				void createPlot();
				void createPlotMatch(const QString& key);
				void createPlotTeam(const QString& key);

				void rangeChanged(double minv, double maxv);

				void getTimes(const QJsonArray& array, double& minv, double& maxv);

				QColor matchRobotColor(xero::scouting::datamodel::Alliance c, int slot);
				void processAlliance(const QJsonArray& arr, xero::scouting::datamodel::Alliance c, std::vector<std::shared_ptr<RobotTrack>>& tracks);
				bool extractOneAlliance(const QJsonArray& arr, xero::scouting::datamodel::Alliance c, int slot, std::shared_ptr<RobotTrack> track);

			private:
				QComboBox* box_;
				QRadioButton* matches_;
				QRadioButton* robot_;
				PathFieldView* field_;
				TimeBoundWidget* slider_;
			};
		}
	}
}

