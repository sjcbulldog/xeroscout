//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

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

