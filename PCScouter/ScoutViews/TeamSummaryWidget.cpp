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
#include <QDebug>
#include <QBoxLayout>
#include <QAction>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			QStringList TeamSummaryWidget::ignore_fields_ =
			{
				"SetNumber",
				"MatchNumber",
				"MatchKey",
				"TeamKey",
				"TeamNumber"
			};

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
			}

			TeamSummaryWidget::~TeamSummaryWidget()
			{
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
				regenerate();
			}

			void TeamSummaryWidget::clearView()
			{
				report_txt_->clear();
			}

			void TeamSummaryWidget::regenerate()
			{
				QString key = box_->currentData().toString();
				if (key != current_team_)
				{
					TeamDataSummary ts;
					auto dm = dataModel();

					if (!dm->createTeamSummaryData(key, report_data_))
					{
						report_txt_->clear();
						report_txt_->append("Error generating team summary");
					}
					else
					{
						const std::shared_ptr<const DataModelTeam> team = dm->findTeamByKey(key);
						report_txt_->clear();
						report_txt_->insertHtml("<h1>Team" + QString::number(team->number()) + " - " + team->name() + "</h1><hr><br>");

						for (auto field : report_data_.fields())
						{
							if (ignore_fields_.contains(field->name()))
								continue;

							report_txt_->insertHtml(field->toHTML());
						}
					}
				}
			}
		}
	}
}
