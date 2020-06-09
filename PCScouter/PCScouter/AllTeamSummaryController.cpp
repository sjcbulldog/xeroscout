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