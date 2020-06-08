//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "AllTeamSummaryController.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

AllTeamSummaryController::AllTeamSummaryController(std::shared_ptr<BlueAlliance> ba, 
	std::shared_ptr<ScoutingDataModel> dm, ScoutingDataSet &ds) : ApplicationController(ba, dm), ds_(ds)
{
	for (auto t : dataModel()->teams())
		keys_.push_back(t->key());
	index_ = 0;
	headers_ = false;

	ds_.clear();
}

AllTeamSummaryController::~AllTeamSummaryController()
{
}

bool AllTeamSummaryController::isDone()
{
	return index_ == keys_.count();
}

void AllTeamSummaryController::run()
{
	if (index_ < keys_.count())
	{
		computeOneTeam(keys_.at(index_++));
		pcnt_ = static_cast<int>((double)index_ / (double)keys_.count() * 100);
	}

	if (index_ == keys_.count())
	{
		emit complete(false);
	}
}

void AllTeamSummaryController::computeOneTeam(const QString& key)
{
	QString query, error;
	ScoutingDataSet teamds;

	auto t = dataModel()->findTeamByKey(key);
	if (t == nullptr)
		return;

	query = "select ";
	bool first = true;
	for (auto f : dataModel()->getMatchFields())
	{
		if (f->type() != FieldDesc::Type::String)
		{
			if (!first)
				query += ",";
			query += f->name();

			first = false;
		}
	}	
	query += " from matches where MatchTeamKey = '" + key + "'";

	if (!dataModel()->createCustomDataSet(teamds, query, error))
		return;

	if (!headers_)
	{
		for (auto f : dataModel()->getTeamFields())
			ds_.addHeader(f);

		for (int col = 0; col < teamds.columnCount(); col++)
			ds_.addHeader(teamds.colHeader(col));

		headers_ = true;
	}

	ds_.newRow();
	for (auto f : dataModel()->getTeamFields())
	{
		QVariant v = t->value(f->name());
		ds_.addData(v);
	}

	for (int col = 0; col < teamds.columnCount(); col++)
	{
		QVariant v = teamds.columnSummary(col);
		ds_.addData(v);
	}
}