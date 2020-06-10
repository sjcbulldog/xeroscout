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

#include "TeamSummaryWidget.h"
#include "DataModelTeam.h"
#include <QDebug>
#include <QBoxLayout>
#include <QAction>
#include <QMenu>
#include <QDesktopServices>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			TeamSummaryWidget::TeamSummaryWidget(QWidget* parent) : QWidget(parent), ViewBase("TeamSummaryWidget")
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
				report_txt_ = new QTextBrowser(report_);
				connect(report_txt_, &QTextBrowser::anchorClicked, this, &TeamSummaryWidget::videoLinkClicked);
				lay->addWidget(report_txt_);

				report_txt_->clear();
				report_txt_->append("No Data");
				report_txt_->setContextMenuPolicy(Qt::CustomContextMenu);
				connect(report_txt_, &QTextBrowser::customContextMenuRequested, this, &TeamSummaryWidget::contextMenu);

				report_txt_->setReadOnly(true);
				report_txt_->setOpenLinks(false);
				report_txt_->setOpenExternalLinks(false);
			}

			TeamSummaryWidget::~TeamSummaryWidget()
			{
			}

			void TeamSummaryWidget::videoLinkClicked(const QUrl& link)
			{
				QDesktopServices::openUrl(link);
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
							box_->addItem(QString::number(t->number()) + " - " + t->nick(), t->key());
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
					html = "<center><h1>" + QString::number(t->number()) + " : " + t->nick() + "</h1></center>";
				}

				return html;
			}

			QString TeamSummaryWidget::generateMatchRecord()
			{
				QString rectoken = "$$$$RECORD$$$$";
				QString html;
				ScoutingDataSet ds;
				QString query;
				QString err;
				const char* prop = "ba_totalPoints";

				query = "select MatchKey from matches where MatchTeamKey = '" + current_team_ + "'";
				if (dataModel()->createCustomDataSet(ds, query, err))
				{
					int won = 0, loss = 0, tie = 0;

					html = "<center><table border=\"1\">";
					html += "<tr><th colspan=\"10\">Matches " + rectoken + " </th></tr>";
					for (int row = 0; row < ds.rowCount(); row++)
					{
						bool isred = true;
						QVariant key = ds.get(row, "MatchKey");
						assert(key.type() == QVariant::String);

						auto m = dataModel()->findMatchByKey(key.toString());
						html += "<tr>";
						html += "<td width=\"80\">";

						QString youtube = m->youtubeKey();

						if (youtube.length() > 0)
						{
							html += "<a href=\"https://www.youtube.com/watch?v=" + youtube + "\">";
						}

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

						if (youtube.length() > 0)
						{
							html += "</a>";
						}
						html += "</td>";

						for (int slot = 1; slot <= 3; slot++)
						{
							QString mkey = m->team(Alliance::Red, slot);
							html += "<td width=\"80\" bgcolor=\"#ff9999\">";
							if (mkey == current_team_)
							{
								html += "<p style=\"text-decoration:underline;font-weight:bold;\">";
								isred = true;
							}
							html += mkey.mid(3);
							if (mkey == current_team_)
							{
								html += "</p>";
							}
							html += "</td>";
						}

						for (int slot = 1; slot <= 3; slot++)
						{
							QString mkey = m->team(Alliance::Blue, slot);
							html += "<td width=\"80\" bgcolor=\"#cee5ff\">";
							if (mkey == current_team_)
							{
								isred = false;
								html += "<p style=\"text-decoration:underline;font-weight:bold;\">";
							}
							html += mkey.mid(3);
							if (mkey == current_team_)
							{
								html += "</p>";
							}
							html += "</td>";
						}

						int bluescore = -1, redscore = -1;
						ConstScoutingDataMapPtr exdata = m->extraData(Alliance::Red, 1);
						auto it = exdata->find(prop);
						if (it != exdata->end())
							redscore = it->second.toInt();

						exdata = m->extraData(Alliance::Blue, 1);
						it = exdata->find(prop);
						if (it != exdata->end())
							bluescore = it->second.toInt();

						if (redscore != -1)
						{
							html += "<td width=\"40\" bgcolor=\"#ff9999\" style=\"text-align:center\">";
							if (redscore > bluescore)
								html += "<b>";
							html += QString::number(redscore);
							if (redscore > bluescore)
								html += "</b>";
							html += "</td>";
						}
						else
						{
							html += "<td width=\"40\"></td>";
						}

						if (bluescore != -1)
						{
							html += "<td width=\"40\" bgcolor=\"#cee5ff\" style=\"text-align:center\">";
							if (redscore < bluescore)
								html += "<b>";
							html += QString::number(bluescore);
							if (redscore < bluescore)
								html += "</b>";
							html += "</td>";
						}
						else
						{
							html += "<td width=\"40\"></td>";
						}

						QString status;

						if (bluescore != -1 && redscore != -1)
						{
							if (bluescore == redscore)
							{
								status = "T";
								tie++;
							}
							else if (isred)
							{
								if (redscore > bluescore)
								{
									won++;
									status = "W";
								}
								else
								{
									status = "L";
									loss++;
								}
							}
							else
							{
								if (redscore < bluescore)
								{
									status = "W";
									won++;
								}
								else
								{
									status = "L";
									loss++;
								}
							}
						}

						html += "<td width=\"40\" style=\"text-align:center\"><b>" + status + "</b></td>";
					}
					html += "</table></center>";

					if (won + loss + tie > 0)
					{
						QString record = QString::number(won) + " - " + QString::number(loss);
						if (tie > 0)
							record += " - " + QString::number(tie);

						record = "( " + record + " )";
						html = html.replace(rectoken, record);
					}
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
						if (col != -1)
							v = matches_ds_.columnSummary(col, true);
					}

					html += "<tr>";
					html += "<td width=\"40\" >" + field + "</td>";

					html += "<td width=\"120\" >";
					if (!v.isValid())
					{
						html += "No Data";
					}
					else if (v.type() == QVariant::Type::Double)
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
