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

#include "AllTeamSummaryGenerator.h"
#include "ScoutingDataModel.h"
#include "TeamDataSummary.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			AllTeamSummaryGenerator::AllTeamSummaryGenerator(std::shared_ptr<ScoutingDataModel> dm, ScoutingDataSet& ds) : ds_(ds)
			{
				index_ = 0;
				state_ = State::TeamSummaries;
				dm_ = dm;
			}

			AllTeamSummaryGenerator::~AllTeamSummaryGenerator()
			{
			}

			void AllTeamSummaryGenerator::start()
			{
				state_ = State::Start;
			}

			void AllTeamSummaryGenerator::createChoicesSet(const std::list<std::shared_ptr<TeamDataSummary>>& list, std::map<QString, QStringList>& choices)
			{
				for (auto summary : list)
				{
					for (auto field : summary->fields())
					{
						auto choice = std::dynamic_pointer_cast<TeamDataSummary::ChoiceField>(field);
						if (choice != nullptr)
						{
							QStringList clist;

							auto it = choices.find(choice->name());
							if (it != choices.end())
								clist = it->second;

							for (const QString& one : choice->choices())
							{
								if (!clist.contains(one))
									clist.push_back(one);
							}

							choices.insert_or_assign(choice->name(), clist);
						}
					}
				}
			}

			void AllTeamSummaryGenerator::generateDataSet()
			{
				bool columns = false;

				ds_.clear();
				for (auto summary : summaries_) {
					if (!columns)
					{
						for (auto field : summary->fields())
						{
							if (field->type() == QVariant::Int)
							{
								ds_.addHeader(field->name(), QVariant::Double);
							}
							else if (field->type() == QVariant::Bool)
							{
								ds_.addHeader(field->name() + "/TRUE", QVariant::Double);
							}
							else if (field->type() == QVariant::String)
							{
								auto choice = std::dynamic_pointer_cast<TeamDataSummary::ChoiceField>(field);
								const QStringList& clist = choices_[field->name()];
								for (const QString& col : clist)
								{
									ds_.addHeader(field->name() + "/" + col, QVariant::Double);
								}
							}
						}
						columns = true;
					}

					ds_.newRow();
					for (auto field : summary->fields())
					{
						if (field->type() == QVariant::Int)
						{
							auto intf = std::dynamic_pointer_cast<TeamDataSummary::IntField>(field);
							ds_.addData(intf->average());
						}
						else if (field->type() == QVariant::Bool)
						{
							auto boolf = std::dynamic_pointer_cast<TeamDataSummary::BooleanField>(field);
							ds_.addData(boolf->average() * 100.0);
						}
						else if (field->type() == QVariant::String)
						{
							auto choice = std::dynamic_pointer_cast<TeamDataSummary::ChoiceField>(field);
							const QStringList& clist = choices_[field->name()];
							for (const QString& col : clist)
								ds_.addData(choice->average(col));
						}
					}
				}
			}

			bool AllTeamSummaryGenerator::run()
			{
				if (state_ == State::Start)
				{
					for (auto t : dm_->teams())
						keys_.push_back(t->key());

					error_.clear();
					summaries_.clear();
					choices_.clear();

					emit reportProgress(0);

					state_ = State::TeamSummaries;
				}
				else if (state_ == State::TeamSummaries)
				{
					QString key = keys_.at(index_++);
					std::shared_ptr<TeamDataSummary> summary = std::make_shared<TeamDataSummary>();

					if (!dm_->createTeamSummaryData(key, *summary))
					{
						error_ = "error generating summary for team " + key;
						return false;
					}

					summary->removeField("Type");
					summary->removeField("SetNumber");
					summary->removeField("MatchNumber");
					summary->removeField("MatchKey");
					summary->removeField("TeamKey");
					summary->removeField("Alliance");
					summary->removeField("Position");
					summary->removeField("match_scouter");

					summaries_.push_back(summary);
					if (index_ == keys_.size())
						state_ = State::CreateChoicesSet;

					double pcnt = ((double)index_ / (double)keys_.size() * 70);
					emit reportProgress(static_cast<int>(pcnt));
				}
				else if (state_ == State::CreateChoicesSet)
				{
					emit reportProgress(80);
					createChoicesSet(summaries_, choices_);
					state_ = State::CreateDataSet;
				}
				else if (state_ == State::CreateDataSet)
				{
					emit reportProgress(100);
					generateDataSet();

					error_.clear();
					summaries_.clear();
					choices_.clear();

					state_ = State::Complete;

					emit reportProgress(100);
				}

				return true;
			}
		}
	}
}
