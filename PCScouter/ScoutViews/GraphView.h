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
#include "GraphDescriptor.h"
#include "ChartViewWrapper.h"
#include "ScoutDataExprContext.h"
#include "ScoutingDataMap.h"
#include "Expr.h"
#include <QWidget>
#include <QComboBox>
#include <QChartView>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT GraphView : public QWidget, public ViewBase
			{
			public:
				GraphView(bool team, bool match, QWidget* parent);
				virtual ~GraphView();

				void setDescriptor(const xero::scouting::datamodel::GraphDescriptor& desc);

				bool hasDescriptor() const {
					return desc_.isValid();
				}

				void refreshCharts();
				void create();

			protected:
				void mousePressEvent(QMouseEvent* ev) override;
				void keyPressEvent(QKeyEvent* ev) override;


				virtual void createTop(QGroupBox* top) = 0;

				bool changeTriggered() { return changed_triggered_; }

				virtual bool generateOneChart(std::shared_ptr<const xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart) = 0;
				bool addDataElements(const QString& query, xero::scouting::datamodel::ScoutingDataMapPtr varvalues, xero::scouting::datamodel::ScoutingDataSet& ds);
				QStringList findAllFieldsUsed(xero::scouting::datamodel::ScoutDataExprContext& ctxt, const QStringList& exprlist, std::vector<std::shared_ptr<xero::expr::Expr>>& exprs);

				std::vector<std::shared_ptr<xero::scouting::datamodel::FieldDesc>> getRequiredFields();

			private:
				static const int constexpr MaxPanes = 9;

				std::shared_ptr<ChartViewWrapper> createForPane(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);

				void generateCharts();

				void newPane();
				void addVariable(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& var);
				void removeVariable(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& var);
				void setRange(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void deletePane(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void addExpr(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);

				void editTitle(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void titleChanged(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& text);

				void setTheme(QChart::ChartTheme theme);

			private:
				QGroupBox* top_;
				QWidget* bottom_;
				QGridLayout* grid_;
				std::vector<std::shared_ptr<ChartViewWrapper>> charts_;
				xero::scouting::datamodel::GraphDescriptor desc_;

				std::list<std::pair<std::shared_ptr<ChartViewWrapper>, std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane>>> pane_chart_map_;

				int count_;
				bool changed_triggered_;
				QChart::ChartTheme theme_;

				bool include_team_;
				bool include_match_;

				static std::list<std::pair<QString, QChart::ChartTheme>> themes_;
			};
		}
	}
}


