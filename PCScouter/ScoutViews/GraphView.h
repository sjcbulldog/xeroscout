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
				GraphView(QWidget* parent);
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

				void setTeamsColors(const QStringList& teams, const QStringList &aug) {
					keys_ = teams; 
					augmentation_ = aug;
				}

				void setTeams(const QStringList& teams) {
					keys_ = teams;
					augmentation_.clear();
				}

				const QStringList& getTeams() const {
					return keys_;
				}

				virtual void createTop(QGroupBox* top) = 0;

				bool changeTriggered() { return changed_triggered_; }

			private:
				static const int constexpr MaxPanes = 9;

				std::shared_ptr<ChartViewWrapper> createForPane(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);

				bool addDataElements(const QString& query, xero::scouting::datamodel::ScoutingDataMapPtr varvalues, xero::scouting::datamodel::ScoutingDataSet& ds);
				QStringList findAllFieldsUsed(xero::scouting::datamodel::ScoutDataExprContext& ctxt, const QStringList& exprlist, std::vector<std::shared_ptr<xero::expr::Expr>>& exprs);

				void generateCharts();
				bool generateOneChart(std::shared_ptr<const xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart, const QStringList& teams);
				bool getData(std::map<QString, std::vector<QVariant>>& data, const QStringList& teams, const QStringList& vars);

				void newPane();
				void addVariable(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& var);
				void removeVariable(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& var);
				void setRange(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void deletePane(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void addExpr(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);

				void editTitle(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane);
				void titleChanged(std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, const QString& text);

				void setTheme(QtCharts::QChart::ChartTheme theme);

			private:
				QGroupBox* top_;
				QWidget* bottom_;
				QGridLayout* grid_;
				std::vector<std::shared_ptr<ChartViewWrapper>> charts_;
				xero::scouting::datamodel::GraphDescriptor desc_;

				QStringList keys_;
				QStringList augmentation_;

				std::list<std::pair<std::shared_ptr<ChartViewWrapper>, std::shared_ptr<xero::scouting::datamodel::GraphDescriptor::GraphPane>>> pane_chart_map_;

				int count_;
				bool changed_triggered_;
				QtCharts::QChart::ChartTheme theme_;

				static std::list<std::pair<QString, QtCharts::QChart::ChartTheme>> themes_;


			};
		}
	}
}


