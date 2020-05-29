#include "NewEventAppController.h"
#include "NewEventBlueAllianceWizard.h"
#include "DataModelBuilder.h"
#include <QString>
#include <QMessageBox>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

NewEventAppController::NewEventAppController(std::shared_ptr<BlueAlliance> ba, const QStringList& tablets, int year) : ApplicationController(ba)
{
	tablets_ = tablets;
	state_ = State::Start;
	year_ = year;
}

NewEventAppController::~NewEventAppController()
{
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
			promptUser();
			break;

		case State::WaitingForMatches:
			gotMatches();
			break;

		case State::WaitingForTeams:
			gotTeams();
			break;
		}
	}
}

void NewEventAppController::gotTeams()
{
	auto teams = blueAlliance()->getEngine().teams();
	for (const QString& teamkey : team_keys_) {
		auto it = teams.find(teamkey);
		if (it == teams.end()) {
			//
			// The key is in the form of frc####, so we extract the number from
			// the key for the team number
			//
			int teamno = teamkey.mid(3).toInt();
			emit logMessage("team '" + teamkey + "' missing from BlueAlliance data");

			QString mockname = "Team #" + QString::number(teamno);
			dm_->addTeam(teamkey, teamno, mockname);
		}
		else {
			dm_->addTeam(it->second->key(), it->second->num(), it->second->nick());
		}
	}

	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches)
	{
		auto m = pair.second;
		if (m->compLevel() != "qm")
			continue;

		std::shared_ptr<DataModelMatch> dm = dm_->addMatch(m->key(), m->compLevel(), m->setNumber(), m->matchNumber(), m->eTime());

		QStringList& red = m->red()->getTeams();
		for (int i = 0; i < red.size(); i++) {
			QString team = m->red()->getTeams().at(i);
			auto t = dm_->findTeamByKey(team);
			dm_->addTeamToMatch(dm->key(), Alliance::Red, i + 1, team, t->number());
		}

		QStringList& blue = m->blue()->getTeams();
		for (int i = 0; i < blue.size(); i++) {
			QString team = m->blue()->getTeams().at(i);
			auto t = dm_->findTeamByKey(team);
			dm_->addTeamToMatch(dm->key(), Alliance::Blue, i + 1, team, t->number());
		}
	}

	// Assign tablets to matches
	dm_->assignMatches();

	// Assign tablets to team pits
	dm_->assignTeams();

	emit complete(false);
	state_ = State::Done;
}

void NewEventAppController::gotMatches()
{
	team_keys_.clear();

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

		dm_ = DataModelBuilder::buildModel(blueAlliance()->getEngine(), wizard.getPitScoutingForm(), wizard.getMatchScoutingForm(), wizard.getEventKey(), error);
		if (dm_ == nullptr)
		{
			dm_ = nullptr;

			emit logMessage("cannot create event data model - " + error);
			emit errorMessage(error);
			emit complete(true);

			state_ = State::Error;
			return;
		}

		if (!dm_->matchScoutingForm()->isOK()) {
			QString line(wizard.getMatchScoutingForm());
			line += ": cannot parse match scouting form";
			emit logMessage(line);

			for (const QString& err : dm_->matchScoutingForm()->errors())
				emit logMessage(err);

			good = false;
		}

		if (dm_->matchScoutingForm()->formType() != "match")
		{
			QString line(wizard.getMatchScoutingForm());
			line += ": wrong type of form, expected 'match', got '";
			line += dm_->matchScoutingForm()->formType() + "'";
			emit logMessage(line);

			good = false;
		}

		if (!dm_->teamScoutingForm()->isOK()) {
			QString line(wizard.getPitScoutingForm());
			line += ": cannot parse team scouting form, ";
			emit logMessage(line);

			for (const QString& err : dm_->teamScoutingForm()->errors())
				emit logMessage(err);

			good = false;
		}

		if (dm_->teamScoutingForm()->formType() != "team")
		{
			QString line(wizard.getMatchScoutingForm());
			line += ": wrong type of form, expected 'team', got '";
			line += dm_->matchScoutingForm()->formType() + "'";
			emit logMessage(line);
			good = false;
		}

		if (!good) {
			dm_ = nullptr;

			emit errorMessage("Invalid scouting forms loaded - see log window");
			emit complete(true);

			state_ = State::Error;
		}
		else {

			dm_->setTabletLists(wizard.getPitTabletList(), wizard.getMatchTabletList());

			state_ = State::WaitingForMatches;
			blueAlliance()->requestMatches(wizard.getEventKey());
		}
	}
	else {
		state_ = State::Done;
		dm_ = nullptr;
	}
}