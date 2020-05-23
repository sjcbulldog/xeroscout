#include "KPIController.h"
#include "ScoutingDataModel.h"
#include "BlueAllianceEngine.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataSet.h"
#include "DataModelBuilder.h"
#include <QDebug>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

KPIController::KPIController(std::shared_ptr<BlueAlliance> ba, int year, const QStringList& teams, 
	const QString& evkey, std::shared_ptr<const ScoutingForm> pit, std::shared_ptr<const ScoutingForm> match) : ApplicationController(ba)
{
	state_ = State::Start;
	teams_ = teams;
	year_ = year;
	evkey_ = evkey;
	pit_ = pit;
	match_ = match;
}

KPIController::~KPIController()
{
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

		auto model = DataModelBuilder::buildModel(blueAlliance()->getEngine(), pit_, match_, eit->second->key(), error);
		assert(model != nullptr);

		//
		// Now find any teams that were at the event
		//
		for (auto team : model->teams())
		{
			if (teams_.contains(team->key()))
			{
				//
				// We have a hit, a team we were interseted in was at another event
				//
				query = "select * from matches where TeamKey='" + team->key() + "'";
				model->createCustomDataSet(ds, query, error);

				//
				// Now process the data set.  Only projess those entries that start with
				// the letters 'ba_' as these come from the blue alliance
				//
				for (int i = 0; i < ds.columnCount(); i++)
				{
					if (!ds.headers().at(i).startsWith("ba_"))
						continue;
				}
			}
		}
	}
}

void KPIController::run()
{
	if (blueAlliance()->isIdle())
	{
		switch (state_)
		{
		case State::Start:
			qDebug() << "Start";
			if (blueAlliance()->getEngine().events().size() == 0)
				getEvents();
			else if (blueAlliance()->getEngine().teams().size() == 0)
				getTeams();
			else
				getTeamEvents();
			break;

		case State::WaitingOnEvents:
			if (blueAlliance()->getEngine().teams().size() == 0)
				getTeams();
			else
				getTeamEvents();
			break;

		case State::WaitingOnTeams:
			getTeamEvents();
			break;

		case State::WaitingOnTeamEvents:
			gotTeamEvents();
			break;

		case State::WaitingOnMatches:
			if (index_ != evlist_.size())
				blueAlliance()->requestMatches(evlist_.at(index_++));
			else
				gotMatches();
			break;

		case State::WaitingOnMatchDetail:
			computeKPI();
			state_ = State::Done;
			break;
		}
	}
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
			matchkeys.push_back(m->key());
	}

	blueAlliance()->requestMatchesDetails(matchkeys);
	state_ = State::WaitingOnMatchDetail;
}

void KPIController::gotTeamEvents()
{
	QDate now = QDate::currentDate();
	auto& bateams = blueAlliance()->getEngine().teams();
	auto& baevents = blueAlliance()->getEngine().events();

	evlist_.clear();

	for (auto team : teams_)
	{
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

				if (evit->second->start() > now)
					continue;

				if (evlist_.contains(ev))
					continue;

				QString st = evit->second->start().toString();
				evlist_.push_back(ev);
			}
		}
	}

	state_ = State::WaitingOnMatches;
	index_ = 0;
	blueAlliance()->requestMatches(evlist_.at(index_++));
}

void KPIController::getEvents()
{
	state_ = State::WaitingOnEvents;
	blueAlliance()->requestEvents(year_);
}

void KPIController::getTeams()
{
	state_ = State::WaitingOnTeams;
	blueAlliance()->requestTeams(teams_);
}

void KPIController::getTeamEvents()
{
	state_ = State::WaitingOnTeamEvents;
	blueAlliance()->requestTeamEvents(teams_, year_);
}

