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

#include "ImportMatchScheduleController.h"
#include "NewEventBlueAllianceWizard.h"
#include "DataModelBuilder.h"
#include <QString>
#include <QMessageBox>
#include <QDebug>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportMatchScheduleController::ImportMatchScheduleController(std::shared_ptr<BlueAlliance> ba, 
				std::shared_ptr<ScoutingDataModel> dm) : ApplicationController(ba, dm)
{
	state_ = State::Start;
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
		emit errorMessage("cannot download match schedule, cannot reach the blue alliance");
		emit logMessage("cannot download match schedule, cannot reach the blue alliance");

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
		emit errorMessage("The BlueAlliance site went down during the operation");
		emit logMessage("The BlueAlliance site went down during the operation");
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
				const QDate& d = dataModel()->startDate();
				qDebug() << "Year: " << d.year();
				qDebug() << "Date: " << d;
				blueAlliance()->requestEvents(d.year());
			}

			state_ = State::WaitingForEvents;
			break;

		case State::WaitingForEvents:
			state_ = State::WaitingForMatches;
			blueAlliance()->requestMatches(dataModel()->evkey());
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
	DataModelBuilder::addTeamsMatches(blueAlliance()->getEngine(), dataModel());

	emit complete(false);
	state_ = State::Done;
}

void ImportMatchScheduleController::noMatches()
{
	emit errorMessage("The match schedule is not available (yet)");
	emit logMessage("The match schedule is not available (yet)");

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
