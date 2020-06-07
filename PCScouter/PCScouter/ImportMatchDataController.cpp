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

#include "ImportMatchDataController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"
#include "TestDataInjector.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportMatchDataController::ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int maxmatch) : ApplicationController(ba)
{
	dm_ = dm;
	state_ = State::Start;
	maxmatch_ = maxmatch;
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
				{
					if (m->match() <= maxmatch_)
						keys.push_back(m->key());
				}

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

void ImportMatchDataController::breakoutBlueAlliancePerRobotData(std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>>& data)
{
	for (auto m : dm_->matches()) {
		if (m->match() <= maxmatch_)
		{
			breakOutBAData(m, Alliance::Red, data[m->key()].first);
			breakOutBAData(m, Alliance::Blue, data[m->key()].second);
		}
	}
}

void ImportMatchDataController::breakOutBAData(std::shared_ptr<const DataModelMatch> m, Alliance c, ScoutingDataMapPtr data)
{
	ScoutingDataMapPtr newdata1 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata2 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata3 = std::make_shared<ScoutingDataMap>();

	for (auto pair : *data)
	{
		if (pair.first.endsWith("Robot1")) {
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata1->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot2"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata2->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot3"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata3->insert_or_assign(name, pair.second);
		}
		else
		{
			newdata1->insert_or_assign(pair.first, pair.second);
			newdata2->insert_or_assign(pair.first, pair.second);
			newdata3->insert_or_assign(pair.first, pair.second);
		}
	}

	newdata1->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));
	newdata2->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));
	newdata3->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));

	dm_->addMatchExtraData(m->key(), c, 1, newdata1);
	dm_->addMatchExtraData(m->key(), c, 2, newdata2);
	dm_->addMatchExtraData(m->key(), c, 3, newdata3);

	dm_->addExtraDataFields(newdata1);
	dm_->addExtraDataFields(newdata2);
	dm_->addExtraDataFields(newdata3);
}



void ImportMatchDataController::gotDetail()
{
	bool baFieldsAdded = false;

	dm_->blockSignals(true);

	//
	// Move the data from the blue alliance engine to the data model
	//
	std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>> badata;

	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches) {
		auto m = dm_->findMatchByKey(pair.first);
		if (m == nullptr)
			continue;

		if (!pair.second->scoreBreakdown().isEmpty())
		{
			if (m->match() <= maxmatch_)
			{
				auto redmap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "red", redmap);

				auto bluemap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "blue", bluemap);

				badata.insert_or_assign(m->key(), std::make_pair(redmap, bluemap));
			}
		}

		if (pair.second->hasVideos())
		{
			dm_->assignVideos(m->key(), pair.second->videos());
		}
	}

	breakoutBlueAlliancePerRobotData(badata);

	dm_->blockSignals(false);

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
	dm_->blockSignals(true);

	auto teams = blueAlliance()->getEngine().teams();
	for (auto team : teams)
	{
		QString key = team.second->key();

		if (!team.second->ranking().isEmpty())
			dm_->setTeamRanking(key, team.second->ranking());
	}

	dm_->blockSignals(false);

	state_ = State::Done;
	emit complete(false);

	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::MatchDataChanged);
	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::TeamDataChanged);
}