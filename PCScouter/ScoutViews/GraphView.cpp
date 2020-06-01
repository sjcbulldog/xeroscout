#include "GraphView.h"
#include "SelectTeamsDialog.h"
#include "ChartViewWrapper.h"
#include "Expr.h"
#include "ScoutDataExprContext.h"
#include "ExpressionsEntryDialog.h"
#include <QBoxLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>

using namespace QtCharts;
using namespace xero::expr;
using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			GraphView::GraphView(QWidget *parent) : QWidget(parent), ViewBase("GraphView")
			{
				bottom_ = nullptr;
				grid_ = nullptr;
				top_ = nullptr;

				setMouseTracking(true);
				setFocusPolicy(Qt::StrongFocus);

				count_ = 1;
				changed_triggered_ = false;
			}

			GraphView::~GraphView()
			{
			}

			void GraphView::keyPressEvent(QKeyEvent* ev)
			{
				if (ev->key() == Qt::Key_Insert && pane_chart_map_.size() < MaxPanes)
					newPane();
			}

			void GraphView::create()
			{
				top_ = new QGroupBox(this);

				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				vlay->addWidget(top_);
				createTop(top_);

				bottom_ = new QWidget(this);
				grid_ = new QGridLayout();
				bottom_->setLayout(grid_);
				vlay->addWidget(bottom_);

				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				bottom_->setSizePolicy(p);
			}

			void GraphView::setDescriptor(const GraphDescriptor &desc) {
				desc_ = desc;
				refreshView();
			}

			void GraphView::newPane()
			{
				desc_.addPane("New Graph " + QString::number(count_++));
				refreshCharts();
				dataModel()->updateGraphDescriptor(desc_);
			}

			void GraphView::editTitle(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				auto chart = createForPane(pane);
				chart->editTitle();
			}

			void GraphView::deletePane(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				std::shared_ptr<ChartViewWrapper> chart = nullptr;

				for (auto it = pane_chart_map_.begin(); it != pane_chart_map_.end(); it++)
				{
					if (it->second == pane)
					{
						chart = it->first;
						qDebug() << "Deleting " << chart->chart()->title();
						grid_->removeWidget(it->first.get());
						pane_chart_map_.erase(it);
						break;
					}
				}
				assert(chart != nullptr);

				for (auto it = charts_.begin(); it != charts_.end(); it++)
				{
					if (*it == chart)
					{
						chart = nullptr;
						charts_.erase(it);
						break;
					}
				}
				assert(chart == nullptr);

				desc_.deletePane(pane);
				refreshCharts();

				changed_triggered_ = true;
				dataModel()->updateGraphDescriptor(desc_);
				changed_triggered_ = false;
			}

			void GraphView::addVariable(std::shared_ptr<GraphDescriptor::GraphPane> pane, const QString &var)
			{
				pane->addX(var);

				refreshCharts();

				changed_triggered_ = true;
				dataModel()->updateGraphDescriptor(desc_);
				changed_triggered_ = false;
			}

			void GraphView::addExpr(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				bool ok;

				QStringList list;
				for (auto f : dataModel()->getAllFields())
				{
					if (f->type() == FieldDesc::Type::Double || f->type() == FieldDesc::Type::Integer || f->type() == FieldDesc::Type::Boolean)
						list.push_back(f->name());
				}
				ExpressionsEntryDialog dialog(list, this);
				if (dialog.exec() == QDialog::Accepted)
				{
					ScoutDataExprContext ctxt(list);
					Expr e;
					QString err;

					if (!e.parse(ctxt, dialog.expr(), err))
					{
						err = "Expressions Error - " + err;
						QMessageBox::critical(this, "Expressions Error", err);
						return;
					}

					pane->addX(dialog.expr());

					refreshCharts();

					changed_triggered_ = true;
					dataModel()->updateGraphDescriptor(desc_);
					changed_triggered_ = false;
				}
			}

			void GraphView::removeVariable(std::shared_ptr<GraphDescriptor::GraphPane> pane, const QString& var)
			{
				pane->removeX(var);

				refreshCharts();

				changed_triggered_ = true;
				dataModel()->updateGraphDescriptor(desc_);
				changed_triggered_ = false;
			}

			void GraphView::setRange(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				bool ok;

				double minv = QInputDialog::getDouble(this, "Minimum", "Minimum", 0.0, 0.0, std::numeric_limits<double>::max(), 1, &ok);
				if (!ok)
					return;

				double maxv = QInputDialog::getDouble(this, "Minimum", "Minimum", 0.0, 0.0, std::numeric_limits<double>::max(), 1, &ok);
				if (!ok)
					return;

				pane->setRange(minv, maxv);

				refreshCharts();

				changed_triggered_ = true;
				dataModel()->updateGraphDescriptor(desc_);
				changed_triggered_ = false;
			}

			void GraphView::mousePressEvent(QMouseEvent* ev)
			{
				if (ev->button() == Qt::RightButton && dataModel() != nullptr)
				{
					QAction* act;
					std::shared_ptr<QtCharts::QChartView> view;
					std::shared_ptr<GraphDescriptor::GraphPane> pane;

					for (auto chart : charts_)
					{
						QRect r(chart->pos(), chart->size());
						if (r.contains(ev->pos()))
						{
							view = chart;

							//
							// Now find the pane that goes with this chart
							//
							for (auto pair : pane_chart_map_)
							{
								if (pair.first == view)
								{
									pane = pair.second;
									break;
								}
							}
							assert(pane != nullptr);
							break;
						}
					}

					QMenu* menu = new QMenu("Graph View");
					if (desc_.count() < MaxPanes)
					{
						act = menu->addAction("Add Pane");
						connect(act, &QAction::triggered, this, &GraphView::newPane);
					}

					auto editcb = std::bind(&GraphView::editTitle, this, pane);
					act = menu->addAction("Edit Title");
					connect(act, &QAction::triggered, editcb);

					if (view != nullptr)
					{
						auto cb = std::bind(&GraphView::addExpr, this, pane);
						act = menu->addAction("Add Expressions");
						connect(act, &QAction::triggered, cb);

						const QStringList& x = pane->x();
						QMenu* sub = new QMenu("Add Variable");

						for (auto f : dataModel()->getAllFields())
						{
							if (!x.contains(f->name()) && (f->type() == FieldDesc::Type::Integer || f->type() == FieldDesc::Type::Double || f->type() == FieldDesc::Type::Boolean))
							{
								act = sub->addAction(f->name());
								auto cb = std::bind(&GraphView::addVariable, this, pane, f->name());
								connect(act, &QAction::triggered, cb);
							}
						}

						menu->addMenu(sub);

						if (pane->x().count() > 0)
						{
							sub = new QMenu("Remove Variable");
							for (const QString& field : pane->x())
							{
								act = sub->addAction(field);
								auto cb = std::bind(&GraphView::removeVariable, this, pane, field);
								connect(act, &QAction::triggered, cb);
							}

							menu->addMenu(sub);
						}

						cb = std::bind(&GraphView::setRange, this, pane);
						act = menu->addAction("Set Range");
						connect(act, &QAction::triggered, cb);
					}

					if (pane_chart_map_.size() > 0)
						act = menu->addAction("Remove Pane");
					auto cb = std::bind(&GraphView::deletePane, this, pane);
					connect(act, &QAction::triggered, cb);

					menu->exec(ev->globalPos());
				}
			}

			void GraphView::refreshCharts()
			{
				if (!desc_.isValid() || desc_.count() == 0 || dataModel() == nullptr)
					return;

				//
				// Generate the charts as described by the descriptor
				//
				generateCharts();

				//
				// We did not succesfully generate any charts
				//
				if (charts_.size() == 0)
					return;

				//
				// Update the chart layouts
				//
				bottom_->setLayout(nullptr);
				delete grid_;

				grid_ = new QGridLayout();
				bottom_->setLayout(grid_);

				switch (charts_.size())
				{
				case 1:
					grid_->addWidget(charts_[0].get(), 0, 0);
					break;

				case 2:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					break;

				case 3:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 1, 0, 1, 2);
					break;

				case 4:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 1, 0);
					grid_->addWidget(charts_[3].get(), 1, 1);
					break;

				case 5:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 0, 2);
					grid_->addWidget(charts_[3].get(), 1, 0);
					grid_->addWidget(charts_[4].get(), 1, 1);
					break;

				case 6:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 0, 2);
					grid_->addWidget(charts_[3].get(), 1, 0);
					grid_->addWidget(charts_[4].get(), 1, 1);
					grid_->addWidget(charts_[5].get(), 1, 2);
					break;

				case 7:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 0, 2);
					grid_->addWidget(charts_[3].get(), 1, 0);
					grid_->addWidget(charts_[4].get(), 1, 1);
					grid_->addWidget(charts_[5].get(), 1, 2);
					grid_->addWidget(charts_[6].get(), 2, 0);
					break;

				case 8:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 0, 2);
					grid_->addWidget(charts_[3].get(), 1, 0);
					grid_->addWidget(charts_[4].get(), 1, 1);
					grid_->addWidget(charts_[5].get(), 1, 2);
					grid_->addWidget(charts_[6].get(), 2, 0);
					grid_->addWidget(charts_[7].get(), 2, 1);
					break;

				case 9:
					grid_->addWidget(charts_[0].get(), 0, 0);
					grid_->addWidget(charts_[1].get(), 0, 1);
					grid_->addWidget(charts_[2].get(), 0, 2);
					grid_->addWidget(charts_[3].get(), 1, 0);
					grid_->addWidget(charts_[4].get(), 1, 1);
					grid_->addWidget(charts_[5].get(), 1, 2);
					grid_->addWidget(charts_[6].get(), 2, 0);
					grid_->addWidget(charts_[7].get(), 2, 1);
					grid_->addWidget(charts_[8].get(), 2, 2);
					break;
				}

				for (int i = 0; i < grid_->columnCount(); i++)
					grid_->setColumnStretch(i, 1);

				for (int i = 0; i < grid_->rowCount(); i++)
					grid_->setRowStretch(i, 1);

				grid_->update();
			}

			void GraphView::titleChanged(std::shared_ptr<GraphDescriptor::GraphPane> pane, const QString& text)
			{
				pane->setTitle(text);
				dataModel()->updateGraphDescriptor(desc_);
			}

			std::shared_ptr<ChartViewWrapper> GraphView::createForPane(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				for (auto pair : pane_chart_map_)
				{
					if (pair.second == pane)
						return pair.first;
				}

				std::shared_ptr<ChartViewWrapper> chart = std::make_shared<ChartViewWrapper>(bottom_, pane);
				connect(chart.get(),& ChartViewWrapper::titleChanged, this, &GraphView::titleChanged);
				charts_.push_back(chart);
				chart->chart()->setDropShadowEnabled(true);
				chart->chart()->setTitle(pane->title());
				chart->setRenderHint(QPainter::Antialiasing);
				pane_chart_map_.push_back(std::make_pair(chart, pane));

				return chart;
			}

			void GraphView::generateCharts()
			{
				for (int i = 0; i < desc_.count(); i++)
				{
					std::shared_ptr<GraphDescriptor::GraphPane> pane = desc_.pane(i);
					std::shared_ptr<ChartViewWrapper> chart = createForPane(pane);

					generateOneChart(pane, chart, keys_);
				}
			}

			bool GraphView::addDataElements(const QString &query, ScoutingDataMapPtr varvalues, ScoutingDataSet& ds)
			{
				QString err;
				if (!dataModel()->createCustomDataSet(ds, query, err))
				{
					// This should never happen

					return false;
				}

				//
				// Now, compute averages for all of the fields we needed
				//
				for (int i = 0; i < ds.columnCount(); i++)
				{
					double total = 0.0;
					for (int row = 0; row < ds.rowCount(); row++)
						total += ds.get(row, i).toDouble();

					varvalues->insert_or_assign(ds.headers().at(i)->name(), QVariant(total / ds.rowCount()));
				}

				return true;
			}

			QStringList GraphView::findAllFieldsUsed(ScoutDataExprContext& ctxt, const QStringList& exprlist, std::vector<std::shared_ptr<Expr>>& exprs)
			{
				QStringList allused;
				QString err;

				//
				// Create the expressions and get the fields needed
				//
				for (const QString& exprtxt : exprlist)
				{
					//
					// Parse expressions against all fields
					//
					auto expr = std::make_shared<Expr>();
					if (!expr->parse(ctxt, exprtxt, err))
					{
						exprs.clear();
						return allused ;
					}

					QStringList used = expr->allVariables();
					for (const QString& var : used)
					{
						if (!allused.contains(var))
							allused.push_back(var);
					}

					exprs.push_back(expr);
				}

				return allused;
			}

			bool GraphView::getData(std::map<QString, std::vector<QVariant>>& data, const QStringList& teams, const QStringList& exprlist)
			{
				ScoutingDataSet ds;
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
				for (const QString& team : teams)
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

			bool GraphView::generateOneChart(std::shared_ptr<const GraphDescriptor::GraphPane> pane, std::shared_ptr<ChartViewWrapper> chart, const QStringList& teams)
			{
				std::map<QString, std::vector<QVariant>> data;
				double ymax = 0;

				chart->chart()->removeAllSeries();
				auto axes = chart->chart()->axes();
				for (auto axis : axes)
					chart->chart()->removeAxis(axis);

				if (!getData(data, teams, pane->x()))
					return false;

				QBarSeries* series = new QBarSeries();

				int cnt = 0;
				for(int i = 0 ; i < pane->x().size() ; i++)
				{
					QString var = pane->x()[i];
					QBarSet* set = new QBarSet(var);
					for (const QString& team : teams)
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
							yval = 0;

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
				for (int i = 0; i < teams.count(); i++)
				{
					QString label = teams.at(i);
					if (label.length() > 3 && label.startsWith("frc"))
						label = label.mid(3);

					if (i < augmentation_.size())
						label += ":" + augmentation_.at(i) ;

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
				chart->chart()->setTheme(QChart::ChartTheme::ChartThemeBlueCerulean);

				return true;
			}
		}
	}
}