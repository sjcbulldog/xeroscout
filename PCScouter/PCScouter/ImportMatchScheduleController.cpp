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

#include "ImportMatchScheduleController.h"
#include "NewEventBlueAllianceWizard.h"
#include "DataModelBuilder.h"
#include <QString>
#include <QMessageBox>
#include <QDebug>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportMatchScheduleController::ImportMatchScheduleController(std::shared_ptr<BlueAlliance> ba, std::shared_ptr<ScoutingDataModel> dm) : ApplicationController(ba)
{
	state_ = State::Start;
	dm_ = dm;
}

ImportMatchScheduleController::~ImportMatchScheduleController()
{
}

bool ImportMatchScheduleController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void ImportMatchScheduleController::run()
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
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		//
		// The network went down in the middle
		//
		emit errorMessage("The BlueAlliance failed during the operation");
		emit logMessage("The BlueAlliance failed during the operation");
		state_ = State::Error;
		emit complete(true);
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
			if (blueAlliance()->getEngine().eventCount() == 0)
			{
				const QDate& d = dm_->startDate();
				qDebug() << "Year: " << d.year();
				qDebug() << "Date: " << d;
				blueAlliance()->requestEvents(d.year());
			}

			state_ = State::WaitingForEvents;
			break;

		case State::WaitingForEvents:
			state_ = State::WaitingForMatches;
			blueAlliance()->requestMatches(dm_->evkey());
			break;

		case State::WaitingForMatches:
			gotMatches();
			break;

		case State::WaitingForTeams:
			gotTeams();
			break;

		case State::NoMatches:
			noMatches();
			break;
		}
	}
}

void ImportMatchScheduleController::gotTeams()
{
	//
	// Create the teams and matches, and assign matchs and teams to tablets
	//
	DataModelBuilder::addTeamsMatches(blueAlliance()->getEngine(), dm_);

	emit complete(false);
	state_ = State::Done;
}

void ImportMatchScheduleController::noMatches()
{
	emit errorMessage("The match schedule is no available (yet)");

	emit complete(false);
	state_ = State::Done;
}

void ImportMatchScheduleController::gotMatches()
{
	team_keys_.clear();

	if (blueAlliance()->getEngine().matches().size() == 0)
	{
		state_ = State::NoMatches;
	}
	else
	{
		for (auto& pair : blueAlliance()->getEngine().matches()) {
			for (auto& team : pair.second->red()->getTeams())
			{
				if (!team_keys_.contains(team))
					team_keys_.push_back(team);
			}

			for (auto& team : pair.second->blue()->getTeams())
			{
				if (!team_keys_.contains(team))
					team_keys_.push_back(team);
			}
		}

		blueAlliance()->requestTeams(team_keys_);
		state_ = State::WaitingForTeams;
	}
}
