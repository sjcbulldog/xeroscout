//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
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
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QListWidget>

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
				void showDetailMatch(const QString& key);
				void showDetailTeam(const QString& key);
				void detailItemChanged(QListWidgetItem* item);

				void matchesSelected(bool checked);
				void robotSelected(bool checked);
				void comboxChanged(int index);

				void createPlot();
				void createPlotMatch(const QString& key);
				void createPlotTeam(const QString& key);

				void createPlotMatchWithKeys(const QString& key);
				void createPlotTeamWithKeys(const QString& key);

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
				QStringList keys_;
				QPushButton* detail_;
				QSplitter* vertical_;
				QSplitter* horizontal_;
				QWidget* hholder_;
				QTreeWidget* info_;
				QListWidget* list_;
			};
		}
	}
}

