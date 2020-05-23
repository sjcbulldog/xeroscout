#include "ImportMatchDataController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportMatchDataController::ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) : ApplicationController(ba)
{
	dm_ = dm;
	state_ = State::Start;
}

ImportMatchDataController::~ImportMatchDataController()
{
}

bool ImportMatchDataController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void ImportMatchDataController::run()
{
	if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		emit errorMessage("cannot create a new event, cannot reach the blue alliance");
		emit logMessage("cannot create a new event, cannot reach the blue alliance");

		emit complete(true);

		state_ = State::Error;
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Initializing)
	{
		//
		// Wait for the blue alliance engine to either be up or down.  There is a timeout
		// in the blue alliance
		//
	}
	else if (blueAlliance()->isIdle())
	{
		//
		// The primary purpose of this state machine is to wait on the blue alliance to finish
		// an operation and then start the next one.  Therefore we only transition when the blue alliance
		// engine is idle, meaning it has completed that last thing we asked it to do
		//

		switch (state_)
		{
		case State::Start:
			blueAlliance()->requestMatches(dm_->evkey());
			state_ = State::WaitingForMatches;
			break;

		case State::WaitingForMatches:
			{
				QStringList keys;

				for (auto m : dm_->matches())
					keys.push_back(m->key());

				state_ = State::WaitingForDetail;
				blueAlliance()->requestMatchesDetails(keys);
			}
			break;

		case State::WaitingForTeams:
			state_ = State::WaitingForRankings;
			blueAlliance()->requestRankings(dm_->evkey());
			break;

		case State::WaitingForDetail:
			gotDetail();
			break;

		case State::WaitingForRankings:
			gotRankings();
			break;
		}
	}
}

void ImportMatchDataController::gotDetail()
{
	//
	// Move the data from the blue alliance engine to the data model
	//
	std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>> badata;

	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches) {
		if (!pair.second->scoreBreakdown().isEmpty())
		{
			auto m = dm_->findMatchByKey(pair.first);
			if (m != nullptr)
			{
				auto redmap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "red", redmap);

				auto bluemap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "blue", bluemap);

				badata.insert_or_assign(m->key(), std::make_pair(redmap, bluemap));
			}
		}
	}

	dm_->breakoutBlueAlliancePerRobotData(badata);

	QStringList teams;
	auto& bateams = blueAlliance()->getEngine().teams();

	for (auto team : dm_->teams())
	{
		auto it = bateams.find(team->key());
		if (it == bateams.end())
			teams.push_back(team->key());
	}

	if (teams.count() > 0)
	{
		state_ = State::WaitingForTeams;
		blueAlliance()->requestTeams(teams);
	}
	else
	{
		state_ = State::WaitingForRankings;
		blueAlliance()->requestRankings(dm_->evkey());
	}
}

void ImportMatchDataController::gotRankings()
{
	auto teams = blueAlliance()->getEngine().teams();
	for (auto team : teams)
	{
		QString key = team.second->key();

		if (!team.second->ranking().isEmpty())
			dm_->setTeamRanking(key, team.second->ranking());
	}

	state_ = State::Done;
	emit complete(false);
}