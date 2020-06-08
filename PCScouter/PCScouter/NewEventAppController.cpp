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

#include "NewEventAppController.h"
#include "NewEventBlueAllianceWizard.h"
#include "DataModelBuilder.h"
#include <QString>
#include <QMessageBox>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

NewEventAppController::NewEventAppController(std::shared_ptr<BlueAlliance> ba, const QStringList& tablets, int year) : ApplicationController(ba, nullptr)
{
	tablets_ = tablets;
	state_ = State::Start;
	year_ = year;
	sim_no_matches_ = false;
}

NewEventAppController::~NewEventAppController()
{
}

bool NewEventAppController::shouldDisableApp()
{
	return state_ == State::WaitingForEvents || state_ == State::WaitingForMatches || state_ == State::WaitingForTeams;
}

bool NewEventAppController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void NewEventAppController::run()
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
		state_ = State::Error;

		emit errorMessage("The BlueAlliance failed during the operation");
		emit logMessage("The BlueAlliance failed during the operation");
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
			start();
			break;

		case State::WaitingForEvents:
			state_ = State::Start;
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

		case State::WaitingForEventTeams:
			gotEventTeams();
			break;

		case State::WaitingForTeamDetail:
			gotTeamDetail();
			break;
		}
	}
}

void NewEventAppController::gotTeamDetail()
{
	auto it = blueAlliance()->getEngine().events().find(dataModel()->evkey());
	if (it == blueAlliance()->getEngine().events().end())
	{
		state_ = State::Done;
		emit complete(false);
	}
	else
	{
		DataModelBuilder::addTeams(blueAlliance()->getEngine(), dataModel(), it->second->teamKeys());
	}

	state_ = State::Done;
	emit complete(false);
}

void NewEventAppController::gotEventTeams()
{
	auto it = blueAlliance()->getEngine().events().find(dataModel()->evkey());
	if (it == blueAlliance()->getEngine().events().end())
	{
		state_ = State::Done;
		emit complete(false);
	}
	else
	{
		state_ = State::WaitingForTeamDetail;
		blueAlliance()->requestTeams(it->second->teamKeys());
	}
}

void NewEventAppController::gotTeams()
{
	//
	// Create the teams and matches, and assign matchs and teams to tablets
	//
	DataModelBuilder::addTeamsMatches(blueAlliance()->getEngine(), dataModel());

	emit complete(false);
	state_ = State::Done;
}

void NewEventAppController::noMatches()
{
	state_ = State::WaitingForEventTeams;
	blueAlliance()->requestEventTeams(dataModel()->evkey());
}

void NewEventAppController::gotMatches()
{
	team_keys_.clear();

	if (blueAlliance()->getEngine().matches().size() == 0 || sim_no_matches_)
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

void NewEventAppController::start()
{
	if (blueAlliance()->getEngine().eventCount() == 0)
	{
		blueAlliance()->requestEvents(year_);
		state_ = State::WaitingForEvents;
	}
	else
	{
		promptUser();
	}
}

void NewEventAppController::promptUser()
{
	NewEventBlueAllianceWizard wizard(blueAlliance()->getEngine());
	wizard.setTabletList(tablets_);

	if (wizard.exec() == QDialog::DialogCode::Accepted) {
		bool good = true;
		QString error;

		auto dm = DataModelBuilder::buildModel(blueAlliance()->getEngine(), wizard.getPitScoutingForm(), wizard.getMatchScoutingForm(), wizard.getEventKey(), error);
		setDataModel(dm);
		if (dataModel() == nullptr)
		{
			dataModel() = nullptr;

			emit logMessage("cannot create event data model - " + error);
			emit errorMessage(error);
			emit complete(true);

			state_ = State::Error;
			return;
		}

		if (!dataModel()->matchScoutingForm()->isOK()) {
			QString line(wizard.getMatchScoutingForm());
			line += ": cannot parse match scouting form";
			emit logMessage(line);

			for (const QString& err : dataModel()->matchScoutingForm()->errors())
				emit logMessage(err);

			good = false;
		}

		if (dataModel()->matchScoutingForm()->formType() != "match")
		{
			QString line(wizard.getMatchScoutingForm());
			line += ": wrong type of form, expected 'match', got '";
			line += dataModel()->matchScoutingForm()->formType() + "'";
			emit logMessage(line);

			good = false;
		}

		if (!dataModel()->teamScoutingForm()->isOK()) {
			QString line(wizard.getPitScoutingForm());
			line += ": cannot parse team scouting form, ";
			emit logMessage(line);

			for (const QString& err : dataModel()->teamScoutingForm()->errors())
				emit logMessage(err);

			good = false;
		}

		if (dataModel()->teamScoutingForm()->formType() != "team")
		{
			QString line(wizard.getMatchScoutingForm());
			line += ": wrong type of form, expected 'team', got '";
			line += dataModel()->matchScoutingForm()->formType() + "'";
			emit logMessage(line);
			good = false;
		}

		if (!good) {
			setDataModel(nullptr);
			emit errorMessage("Invalid scouting forms loaded - see log window");
			emit complete(true);

			state_ = State::Error;
		}
		else {

			dataModel()->setTabletLists(wizard.getPitTabletList(), wizard.getMatchTabletList());

			state_ = State::WaitingForMatches;
			blueAlliance()->requestMatches(wizard.getEventKey());
		}
	}
	else {
		state_ = State::Done;
		setDataModel(nullptr);
	}
}