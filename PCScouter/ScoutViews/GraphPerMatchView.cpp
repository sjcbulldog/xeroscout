#include "GraphPerMatchView.h"
#include "Expr.h"
#include "ScoutingDataSet.h"
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

using namespace xero::scouting::datamodel;
using namespace xero::expr;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			bool GraphPerMatchView::getData(ScoutingDataSet &ds, const QStringList& exprlist)
			{

				QString err;

				QStringList matchfields;
				for (auto f : dataModel()->getMatchFields())
				{
					if (f->type() == FieldDesc::Type::Double || f->type() == FieldDesc::Type::Integer || f->type() == FieldDesc::Type::Boolean)
						matchfields.push_back(f->name());
				}
				ScoutDataExprContext ctxt(matchfields);

				std::vector<std::shared_ptr<Expr>> exprs;

				QStringList allused;
				allused = findAllFieldsUsed(ctxt, exprlist, exprs);
				if (exprs.size() == 0)
					return false;

				QString query;
				ds.clear();
				query = "SELECT Type, SetNum, MatchNum";

				int count = 3;
				for (int i = 0; i < allused.count(); i++)
				{
					const QString var = allused.at(i);
					if (!matchfields.contains(var))
						continue;

					if (count != 0)
						query += ",";

					query += allused.at(i);
					count++;
				}

				if (count > 0)
				{
					query += " from matches where " + QString(DataModelMatch::MatchTeamKeyName) + "='" + team_ + "'";
					if (!dataModel()->createCustomDataSet(ds, query, err))
					{
						return false;
					}
				}

				return true;
			}

			bool GraphPerMatchView::generateOneChart(std::shared_ptr<const xero::scouting::datamodel::GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart)
			{
				int prefetch = 3;

				chart->chart()->removeAllSeries();
				auto axes = chart->chart()->axes();
				for (auto axis : axes)
					chart->chart()->removeAxis(axis);

				ScoutingDataSet ds("$graph");
				if (!getData(ds, pane->x()))
					return false;

				double ymax = 0.0;

				QBarSeries* series = new QBarSeries();
				for (int i = 0; i < pane->x().size(); i++)
				{
					QString var = pane->x()[i];
					QBarSet* set = new QBarSet(var);

					for (int row = 0; row < ds.rowCount(); row++) {
						QVariant yv = ds.get(row, i + prefetch).toDouble();
						double yval = 0.0;

						if (yv.type() == QVariant::Type::Int || yv.type() == QVariant::Type::Double) {
							yval = yv.toDouble();
						}

						*set << yval;
						if (yval > ymax)
							ymax = yval;
					}

					series->append(set);
				}

				chart->chart()->addSeries(series);
				chart->chart()->setAnimationOptions(QChart::SeriesAnimations);

				QBarCategoryAxis* axisX = new QBarCategoryAxis();
				for (int i = 0; i < ds.rowCount(); i++)
				{
					QString label;
					
					label += ds.get(i, 0).toString();
					label += "-" + QString::number(ds.get(i, 1).toInt());
					label += "-" + QString::number(ds.get(i, 2).toInt());
					axisX->append(label);
				}
				chart->chart()->addAxis(axisX, Qt::AlignBottom);
				series->attachAxis(axisX);

				QValueAxis* axisY = new QValueAxis();
				if (pane->hasRange())
				{
					axisY->setRange(pane->minv(), pane->maxv());
				}
				else
				{
					axisY->setRange(0, ymax);
					axisY->applyNiceNumbers();
				}

				chart->chart()->addAxis(axisY, Qt::AlignLeft);
				series->attachAxis(axisY);

				chart->chart()->legend()->setVisible(true);
				chart->chart()->legend()->setAlignment(Qt::AlignBottom);

				connect(series, &QBarSeries::hovered, chart.get(), &ChartViewWrapper::mouseHovered);

				chart->chart()->legend()->setVisible(true);

				return true;
			}
		}
	}
}
