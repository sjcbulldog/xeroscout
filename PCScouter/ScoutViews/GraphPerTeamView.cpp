#include "GraphPerTeamView.h"
#include "Expr.h"
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
			bool GraphPerTeamView::getData(std::map<QString, std::vector<QVariant>>& data, const QStringList& exprlist)
			{
				ScoutingDataSet ds("$graph");
				QString err;

				QStringList matchfields, alllist;
				ScoutingDataMapPtr varvalues = std::make_shared<ScoutingDataMap>();

				for (auto f : dataModel()->getAllFields())
				{
					if (f->type() == FieldDesc::Type::Double || f->type() == FieldDesc::Type::Integer || f->type() == FieldDesc::Type::Boolean)
						alllist.push_back(f->name());
				}
				ScoutDataExprContext ctxt(alllist);

				for (auto f : dataModel()->getMatchFields())
				{
					if (f->type() == FieldDesc::Type::Double || f->type() == FieldDesc::Type::Integer || f->type() == FieldDesc::Type::Boolean)
						matchfields.push_back(f->name());
				}

				data.clear();
				for (const QString& team : team_keys_)
				{
					std::vector<std::shared_ptr<Expr>> exprs;
					QStringList allused;

					allused = findAllFieldsUsed(ctxt, exprlist, exprs);
					if (exprs.size() == 0)
						return false;

					//
					// For the match fields, find the averages we need
					//
					QString query;
					ds.clear();
					query = "SELECT ";
					int count = 0;
					for (int i = 0; i < allused.count(); i++)
					{
						const QString var = allused.at(i);
						if (!matchfields.contains(var))
							continue;

						if (i != 0)
							query += ",";

						query += allused.at(i);
						count++;
					}

					if (count > 0)
					{
						query += " from matches where " + QString(DataModelMatch::MatchTeamKeyName) + "='" + team + "'";
						if (!addDataElements(query, varvalues, ds))
						{
							data.clear();
							return false;
						}
					}

					//
					// Now, append the team data elements to the set
					//
					query.clear();
					ds.clear();
					query = "SELECT ";
					count = 0;
					for (int i = 0; i < allused.count(); i++)
					{
						const QString var = allused.at(i);
						if (matchfields.contains(var))
							continue;

						if (i != 0)
							query += ",";

						query += allused.at(i);
						count++;
					}

					if (count > 0)
					{
						query += " from teams where " + QString(DataModelTeam::TeamKeyName) + "='" + team + "'";
						if (!addDataElements(query, varvalues, ds))
						{
							data.clear();
							return false;
						}
					}

					ctxt.addValues(varvalues);
					std::vector<QVariant> values;
					for (auto e : exprs)
					{
						QVariant v = e->eval(ctxt);
						values.push_back(v);
					}

					data.insert_or_assign(team, values);
				}
				return true;
			}

			bool GraphPerTeamView::generateOneChart(std::shared_ptr<const GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart)
			{
				std::map<QString, std::vector<QVariant>> data;
				double ymax = 0;

				chart->chart()->removeAllSeries();
				auto axes = chart->chart()->axes();
				for (auto axis : axes)
					chart->chart()->removeAxis(axis);

				if (!getData(data, pane->x()))
					return false;

				QBarSeries* series = new QBarSeries();

				int cnt = 0;
				for (int i = 0; i < pane->x().size(); i++)
				{
					QString var = pane->x()[i];
					QBarSet* set = new QBarSet(var);
					for (const QString& team : team_keys_)
					{
						double yval;
						const std::vector<QVariant>& one = data[team];

						if (one[i].type() == QVariant::Type::Int || one[i].type() == QVariant::Type::Double)
						{
							yval = one[i].toDouble();
						}
						else if (one[i].type() == QVariant::Type::Bool)
						{
							if (one[i].toBool())
								yval = 1.0;
							else
								yval = 0.0;
						}
						else
						{
							yval = 0.0;
						}

						if (isnan(yval) || isinf(yval))
							yval = 0.0;

						*set << yval;
						cnt++;
						if (yval > ymax)
							ymax = yval;
					}
					series->append(set);
				}
				chart->chart()->addSeries(series);
				chart->chart()->setAnimationOptions(QChart::SeriesAnimations);

				connect(series, &QBarSeries::hovered, chart.get(), &ChartViewWrapper::mouseHovered);

				QBarCategoryAxis* axisX = new QBarCategoryAxis();
				for (int i = 0; i < team_keys_.count(); i++)
				{
					QString label = team_keys_.at(i);
					if (label.length() > 3 && label.startsWith("frc"))
						label = label.mid(3);

					if (i < augmentation_.size())
						label += ":" + augmentation_.at(i);

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

				return true;
			}
		}
	}
}
