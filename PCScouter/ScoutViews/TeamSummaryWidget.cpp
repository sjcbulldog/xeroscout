//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "TeamSummaryWidget.h"
#include "DataModelTeam.h"
#include <QDebug>
#include <QBoxLayout>
#include <QAction>
#include <QMenu>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			TeamSummaryWidget::TeamSummaryWidget(QWidget* parent) : QWidget(parent)
			{
				QBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				QWidget* top_form = new QWidget();
				lay->addWidget(top_form);

				QBoxLayout* formlay = new QHBoxLayout();
				top_form->setLayout(formlay);

				box_ = new QComboBox(top_form);
				formlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &TeamSummaryWidget::teamChanged);

				report_ = new QGroupBox("Report", this);
				lay->addWidget(report_);

				lay = new QVBoxLayout();
				report_->setLayout(lay);
				report_txt_ = new QTextEdit(report_);
				lay->addWidget(report_txt_);

				report_txt_->clear();
				report_txt_->append("No Data");
				report_txt_->setContextMenuPolicy(Qt::CustomContextMenu);
				connect(report_txt_, &QTextEdit::customContextMenuRequested, this, &TeamSummaryWidget::contextMenu);

				report_txt_->setReadOnly(true);
			}

			TeamSummaryWidget::~TeamSummaryWidget()
			{
			}

			void TeamSummaryWidget::contextMenu(const QPoint& pt)
			{
				QAction* act;
				auto addfields = dataModel()->getTeamFields();
				QStringList rmfields = dataModel()->teamSummaryFields();

				QPoint p = report_txt_->mapToGlobal(pt);
				QMenu* menu = new QMenu("Team Summary");
				
				QMenu* vars = new QMenu("Add Team Value");
				for (auto f : addfields)
				{
					if (!rmfields.contains(f->name())) {
						auto cb = std::bind(&TeamSummaryWidget::addVariable, this, f->name());
						act = vars->addAction(f->name());
						connect(act, &QAction::triggered, this, cb);
					}
				}
				menu->addMenu(vars);

				if (matches_ds_.columnCount() > 0)
				{
					vars = new QMenu("Add Match Value");
					for (int i = 0; i < matches_ds_.columnCount(); i++)
					{
						auto hdr = matches_ds_.colHeader(i);
						if (!rmfields.contains(hdr->name())) {
							auto cb = std::bind(&TeamSummaryWidget::addVariable, this, hdr->name());
							act = vars->addAction(hdr->name());
							connect(act, &QAction::triggered, this, cb);
						}
					}

					menu->addMenu(vars);
				}

				if (rmfields.size() > 0)
				{
					vars = new QMenu("Remove Variable");
					for (auto e : rmfields)
					{
						auto cb = std::bind(&TeamSummaryWidget::removeVariable, this, e);
						act = vars->addAction(e);
						connect(act, &QAction::triggered, this, cb);
					}

					menu->addMenu(vars);
				}

				menu->exec(p);
			}

			void TeamSummaryWidget::addVariable(const QString& var)
			{
				QStringList list = dataModel()->teamSummaryFields();
				list.push_back(var);
				dataModel()->setTeamSummaryFields(list);
				regenerate(true);
			}

			void TeamSummaryWidget::removeVariable(const QString& var)
			{
				QStringList list = dataModel()->teamSummaryFields();
				list.removeOne(var);
				dataModel()->setTeamSummaryFields(list);
				regenerate(true);
			}

			void TeamSummaryWidget::showEvent(QShowEvent* ev)
			{
				if (box_->count() == 0)
				{
					auto dm = dataModel();
					if (dm != nullptr) {
						std::list<std::shared_ptr<const DataModelTeam>> teams = dm->teams();
						teams.sort([](std::shared_ptr<const DataModelTeam> a, std::shared_ptr<const DataModelTeam> b) -> bool
							{
								return a->number() < b->number();
							});

						for (auto t : teams)
							box_->addItem(QString::number(t->number()) + " - " + t->name(), t->key());
					}
				}
			}

			void TeamSummaryWidget::teamChanged(int index)
			{
				regenerate();
			}

			void TeamSummaryWidget::refreshView()
			{
				regenerate(true);
			}

			void TeamSummaryWidget::clearView()
			{
				report_txt_->clear();
			}

			QString TeamSummaryWidget::generateTitle()
			{
				QString html;

				auto t = dataModel()->findTeamByKey(current_team_);
				if (t == nullptr)
				{
					html = "<center><h1>invalid team key '" + current_team_ + "'</h1></center><br><br>";
				}
				else
				{
					html = "<center><h1>" + QString::number(t->number()) + " : " + t->name() + "</h1></center>";
				}

				return html;
			}

			QString TeamSummaryWidget::generateMatchRecord()
			{
				QString html;
				ScoutingDataSet ds;
				QString query;
				QString err;
				const char* prop = "ba_totalPoints";

				query = "select MatchKey from matches where MatchTeamKey = '" + current_team_ + "'";
				if (dataModel()->createCustomDataSet(ds, query, err))
				{
					html = "<center><table border=\"1\">";
					html += "<tr><th colspan=\"9\">Matches</th></tr>";
					for (int row = 0; row < ds.rowCount(); row++)
					{
						QVariant key = ds.get(row, "MatchKey");
						assert(key.type() == QVariant::String);

						auto m = dataModel()->findMatchByKey(key.toString());
						html += "<tr>";
						html += "<td width=\"80\">";
						if (m->compType() == "qm")
						{
							html += "Quals " + QString::number(m->match());
						}
						else if (m->compType() == "qf")
						{
							html += "Quarters " + QString::number(m->set()) + " Match " + QString::number(m->match());
						}
						else if (m->compType() == "sf")
						{
							html += "Semis " + QString::number(m->set()) + " Match " + QString::number(m->match());

						}
						else if (m->compType() == "f")
						{
							html += "Finals " + QString::number(m->set()) + " Match " + QString::number(m->match());

						}
						else if (m->compType() == "ef")
						{
							html += "Einsteins " + QString::number(m->set()) + " Match " + QString::number(m->match());
						}
						html += "</td>";

						for (int slot = 1; slot <= 3; slot++)
						{
							QString mkey = m->team(Alliance::Red, slot);
							html += "<td width=\"80\" bgcolor=\"#ff9999\">";
							html += mkey.mid(3);
							html += "</td>";
						}

						for (int slot = 1; slot <= 3; slot++)
						{
							QString mkey = m->team(Alliance::Blue, slot);
							html += "<td width=\"80\" bgcolor=\"#cee5ff\">";
							html += mkey.mid(3);
							html += "</td>";
						}

						ConstScoutingDataMapPtr exdata = m->extraData(Alliance::Red, 1);
						auto it = exdata->find(prop);
						if (it != exdata->end())
						{
							html += "<td width=\"40\" bgcolor=\"#ff9999\">" + it->second.toString() +  "</td>";
						}
						else
						{
							html += "<td width=\"40\"></td>";
						}

						exdata = m->extraData(Alliance::Blue, 1);
						it = exdata->find(prop);
						if (it != exdata->end())
						{
							html += "<td width=\"40\" bgcolor=\"#cee5ff\">" + it->second.toString() + "</td>";
						}
						else
						{
							html += "<td width=\"40\"></td>";
						}
					}
					html += "</table></center>";
				}
				else
				{
					html = "<bold>match query failed '" + err + "'</bold>";
				}

				return html;
			}

			QString TeamSummaryWidget::generateTeamSummary()
			{
				QString html;
				QStringList fields = dataModel()->teamSummaryFields();

				auto t = dataModel()->findTeamByKey(current_team_);
				if (t == nullptr)
					return html;

				html = "<center><table border=\"1\">";
				html += "<tr><th colspan=\"2\">TeamInfo</th></tr>";

				for (const QString& field : fields) {
					QVariant v;

					if (t->hasValue(field))
						v = t->value(field);
					else
					{
						int col = matches_ds_.getColumnByName(field);
						v = matches_ds_.columnSummary(col);
					}

					if (v.isValid())
					{
						html += "<tr>";
						html += "<td width=\"40\" >" + field + "</td>";

						html += "<td width=\"120\" >";
						if (v.type() == QVariant::Type::Double)
						{
							html += QString::number(v.toDouble(), 'f', 2);
						}
						else
						{
							html += v.toString();
						}

						html += "</td>";
						html += "</tr>";
					}
				}

				html += "</table>";

				return html;
			}

			void TeamSummaryWidget::createMatchesDataSet()
			{
				QString query, error;

				query = "select * from matches where MatchTeamKey='" + current_team_ + "'";
				if (!dataModel()->createCustomDataSet(matches_ds_, query, error))
					return;
			}

			void TeamSummaryWidget::regenerate(bool force)
			{
				QString key = box_->currentData().toString();
				if (key != current_team_ || force)
				{
					QString html;

					current_team_ = key;
					createMatchesDataSet();

					report_txt_->clear();
					html = generateTitle();
					html += "<hr>";
					html += generateTeamSummary();
					html += "<hr>";
					html += generateMatchRecord();

					report_txt_->setHtml(html);
				}
			}
		}
	}
}
