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

#include "KPIController.h"
#include "ScoutingDataModel.h"
#include "BlueAllianceEngine.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataSet.h"
#include "DataModelBuilder.h"
#include "OPRCalculator.h"
#include <QDebug>
#include <QStandardPaths>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

KPIController::KPIController(std::shared_ptr<BlueAlliance> ba, std::shared_ptr<ScoutingDataModel> dm, const QDate &evdate, const QStringList& teams, 
	const QString& evkey, std::shared_ptr<const ScoutingForm> team, std::shared_ptr<const ScoutingForm> match) : ApplicationController(ba)
{
	evdate_ = evdate;
	state_ = State::Start;
	teams_ = teams;
	evkey_ = evkey;
	team_ = team;
	match_ = match;
	dm_ = dm;
}

KPIController::~KPIController()
{
}

ScoutingDataMapPtr KPIController::evToData(const QString& tkey, const QString& evkey)
{
	qDebug() << "evToData: team key " << tkey << " event key " << evkey;
	if (tkey == "frc1425" && evkey == "2019orore")
		qDebug() << "Break";

	auto it = models_.find(evkey);
	if (it == models_.end())
	{
		qDebug() << "  no such event - returned null";
		return nullptr;
	}
	ScoutingDataMapPtr ret = std::make_shared<ScoutingDataMap>();
	auto model = it->second;

	auto team = model->findTeamByKey(tkey);
	if (team->hasExtraData())
	{
		auto data = team->extraData();
		auto it = data->find(DataModelTeam::OPRName);
		if (it != data->end())
			ret->insert_or_assign(DataModelTeam::OPRName, it->second);
	}

	model->clearDatabaseTables();

	QString query, error;
	ScoutingDataSet ds;
	query = "select * from matches where MatchTeamKey='" + tkey + "'";
	if (!model->createCustomDataSet(ds, query, error))
	{
		qDebug() << "  query failed ";
		emit logMessage("Error runing SQL query '" + query + "' - " + error);
	}
	else
	{
		qDebug() << "  dataset generated " << ds.columnCount() << " columns " << ds.rowCount() << " rows";
	}

	for (int i = 0; i < ds.columnCount(); i++)
	{
		auto hdr = ds.colHeader(i);
		if (!hdr->name().startsWith("ba_"))
			continue;

		QVariant v = ds.columnSummary(i, false);
		if (v.type() == QVariant::Type::String)
			continue;

		ret->insert_or_assign(hdr->name(), v);
	}

	return ret;
}

void KPIController::computeKPI()
{
	auto& baevents = blueAlliance()->getEngine().events();
	ScoutingDataSet ds;
	QString query, error;

	for (const QString& ev : evlist_)
	{
		//
		// Create a data model for the event
		//
		auto eit = baevents.find(ev);
		if (eit == baevents.end())
			continue;

		auto model = DataModelBuilder::buildModel(blueAlliance()->getEngine(), team_, match_, eit->second->key(), error);
		assert(model != nullptr);

		DataModelBuilder::addBlueAllianceData(blueAlliance(), model, std::numeric_limits<int>::max());

		models_.insert_or_assign(ev, model);
		OPRCalculator calc(model);
		if (!calc.calc())
		{
			qDebug() << "Error calculating OPR for event '" << ev << "'";
		}
	}

	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
	for (const QString& ev : evlist_)
	{
		auto model = models_.find(ev);
		if (model != models_.end())
		{
			QString filename = path + "/events/" + ev + ".json";
			model->second->save(filename);
		}
		else
		{
			qDebug() << "KPI: " << ev << " - no model found";
		}
	}

	//
	// Now we have fetched everything and calculated everything so lets get it
	// into the team data for each active team
	//

	for (auto pair : team_event_map_)
	{
		std::map<QString, std::pair<int, double>> calcs;
		for (const QString& ev : pair.second)
		{
			auto data = evToData(pair.first, ev);
			for (auto datapair : *data)
			{
				std::pair<int, double> data = std::make_pair(0, 0.0);
				auto it = calcs.find(datapair.first);
				if (it != calcs.end())
					data = it->second;

				data.first++;
				data.second += datapair.second.toDouble();

				calcs.insert_or_assign(datapair.first, data);
			}
		}

		for (auto pairs : calcs)
		{
			dm_->addTeamExtraData(pair.first, "prev_" + pairs.first, (double)pairs.second.second / (double)pairs.second.first);
		}
	}
}

void KPIController::run()
{
	if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		emit errorMessage("cannot get KPI data, cannot reach the blue alliance");
		emit logMessage("cannot get KPI data, cannot reach the blue alliance");

		state_ = State::Error;

		emit complete(true);
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Initializing)
	{
		//
		// Wait for the blue alliance engine to either be up or down.  There is a timeout
		// in the blue alliance
		//
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		//
		// The network went down in the middle
		//
		state_ = State::Error;

		emit errorMessage("The BlueAlliance failed during the operation");
		emit logMessage("The BlueAlliance failed during the operation");
		emit complete(true);

	}
	else if (blueAlliance()->isIdle())
	{
		switch (state_)
		{
		case State::Start:
			timer_.start();
			qDebug() << "KPI: starting";
			if (blueAlliance()->getEngine().events().size() == 0)
			{
				qDebug() << "KPI: requesting events";
				getEvents();
			}
			else if (blueAlliance()->getEngine().teams().size() == 0)
			{
				qDebug() << "KPI: requesting teams";
				getTeams();
			}
			else
			{
				qDebug() << "KPI: requesting team events";
				getTeamEvents();
			}
			break;

		case State::WaitingOnEvents:
			qDebug() << "KPI: got events";
			if (blueAlliance()->getEngine().teams().size() == 0)
			{
				qDebug() << "KPI: requesting teams";
				getTeams();
			}
			else
			{
				qDebug() << "KPI: requesting team events";
				getTeamEvents();
			}
			break;

		case State::WaitingOnTeams:
			qDebug() << "KPI: got teams";
			qDebug() << "KPI: requesting team events";
			getTeamEvents();
			break;

		case State::WaitingOnTeamEvents:
			qDebug() << "KPI: got team events";
			gotTeamEvents();
			break;

		case State::WaitingOnMatches:
			qDebug() << "KPI: got matches " << index_ << " or " << evlist_.size();
			if (index_ != evlist_.size())
				blueAlliance()->requestMatches(evlist_.at(index_++));
			else
				gotMatches();
			break;

		case State::WaitingOnMatchDetail:
			gotMatchDetail();
			break;

		case State::WaitingOnTeamsPhase2:
			qDebug() << "KPI: got teams phase 2";
			computeKPI();
			emit logMessage("KPI complete, " + QString::number(timer_.elapsed() / 1000) + " seconds");
			state_ = State::Done;
			break;
		}
	}
}

void KPIController::gotMatchDetail()
{
	QStringList tmlist;

	for (auto evpair : blueAlliance()->getEngine().events())
	{
		for (auto match : evpair.second->matches())
		{
			for (auto team : match->red()->getTeams())
			{
				auto it = blueAlliance()->getEngine().teams().find(team);
				if (it != blueAlliance()->getEngine().teams().end())
					continue;

				if (!tmlist.contains(team))
					tmlist.push_back(team);
			}
			for (auto team : match->blue()->getTeams())
			{
				auto it = blueAlliance()->getEngine().teams().find(team);
				if (it != blueAlliance()->getEngine().teams().end())
					continue;

				if (!tmlist.contains(team))
					tmlist.push_back(team);
			}
		}
	}

	state_ = State::WaitingOnTeamsPhase2;
	qDebug() << "KPI: requesting teams (in events but not in target event)";
	blueAlliance()->requestTeams(tmlist);
}

void KPIController::gotMatches()
{
	QStringList matchkeys;
	auto& baevents = blueAlliance()->getEngine().events();

	for (const QString& ev : evlist_)
	{
		auto it = baevents.find(ev);
		if (it == baevents.end())
			continue;

		auto &mlist = it->second->matches();
		for (auto& m : mlist)
		{
			if (m->scoreBreakdown().isEmpty())
				matchkeys.push_back(m->key());
		}
	}

	qDebug() << "KPI: requesting match detail";
	blueAlliance()->requestMatchesDetails(matchkeys);
	state_ = State::WaitingOnMatchDetail;
}

void KPIController::gotTeamEvents()
{
	auto& bateams = blueAlliance()->getEngine().teams();
	auto& baevents = blueAlliance()->getEngine().events();

	evlist_.clear();

	for (auto team : teams_)
	{
		QStringList evs;
		auto it = bateams.find(team);
		if (it != bateams.end())
		{
			QStringList tevlist = it->second->events();
			for (const QString &ev : tevlist)
			{
				if (ev == evkey_)
					continue;

				auto evit = baevents.find(ev);
				if (evit == baevents.end())
					continue;

				if (evit->second->start() > evdate_)
					continue;

				if (!evs.contains(ev))
					evs.push_back(ev);

				if (evlist_.contains(ev))
					continue;

				evlist_.push_back(ev);
			}
		}

		team_event_map_.insert_or_assign(team, evs);
	}

	qDebug() << "KPI: requesting matches";
	state_ = State::WaitingOnMatches;
	index_ = 0;
	blueAlliance()->requestMatches(evlist_.at(index_++));
}

void KPIController::getEvents()
{
	state_ = State::WaitingOnEvents;
	int year = evdate_.year();
	blueAlliance()->requestEvents(year);
}

void KPIController::getTeams()
{
	state_ = State::WaitingOnTeams;
	blueAlliance()->requestTeams(teams_);
}

void KPIController::getTeamEvents()
{
	state_ = State::WaitingOnTeamEvents;
	int year = evdate_.year();
	blueAlliance()->requestTeamEvents(teams_, year);
}

