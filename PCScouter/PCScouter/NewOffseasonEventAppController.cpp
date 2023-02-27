#include "NewOffseasonEventAppController.h"
#include "NewEventOffseasonWizard.h"
#include "DataModelBuilder.h"
#include "CsvReader.h"
#include "PCScouter.h"
#include <filesystem>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

NewOffseasonEventAppController::NewOffseasonEventAppController(ImageManager& mgr, const QStringList& tablets, int year) : ApplicationController(nullptr, nullptr), images_(mgr)
{
	tablets_ = tablets;
	state_ = State::Start;
	year_ = year;
}

NewOffseasonEventAppController::~NewOffseasonEventAppController()
{
}

bool NewOffseasonEventAppController::shouldDisableApp()
{
	return state_ == State::NeedMatches || state_ == State::NeedTeams;
}

bool NewOffseasonEventAppController::isDone()
{
	return state_ == State::Done;
}

void NewOffseasonEventAppController::run()
{
	switch (state_) {
	case State::Start:
		promptUser();
		break;
	}
}

void NewOffseasonEventAppController::promptUser()
{
	NewEventOffseasonWizard wizard(images_, tablets_);

	if (wizard.exec() == QDialog::DialogCode::Accepted) {
		bool good = true;
		QString error;

		auto dm = DataModelBuilder::buildModel(wizard.getPitScoutingForm(), wizard.getMatchScoutingForm(), 
			wizard.getEventKey(), wizard.getEventName(), error);

		setDataModel(dm);
		if (dataModel() == nullptr)
		{
			emit logMessage("cannot create event data model - " + error);
			emit errorMessage(error);
			emit complete(true);

			setDataModel(nullptr);

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

			//
			// Parse the roster spreadsheet and add the teams
			//
			addTeamFromRoster(wizard.getRosterForm());

			dataModel()->setTabletLists(wizard.getPitTabletList(), wizard.getMatchTabletList());
			tablets_ = wizard.getTabletList();

			emit complete(false);

			state_ = State::Done;
		}
	}
	else {
		state_ = State::Done;
		setDataModel(nullptr);
	}
}

bool NewOffseasonEventAppController::addTeamFromRoster(QString roster)
{
	CsvReader reader(true);

	std::filesystem::path rpath(roster.toStdString());
	if (!reader.readFile(rpath, CsvReader::HeaderType::Headers))
		return false;

	if (reader.colCount() < 2) {
		emit errorMessage("Invalid teams file - must have at least two columns");
		return false;
	}

	auto dm = dataModel();
	for (int i = 0; i < reader.rowCount(); i++) {
		const DataRow &row = reader.getRow(i);

		QString key, nick, name, city, state, country;
		int number;

		if (!std::holds_alternative<double>(row[0]))
		{
			QString msg("Invalid teams file - row ");
			msg += QString::number(i + 1) + ": column 1: not an integer value - must be a team number";
			emit errorMessage(msg);
			return false;
		}

		number = static_cast<int>(std::get<double>(row[0]));
		key = "frc" + QString::number(number);

		nick = QString::fromStdString(std::get<std::string>(row[1]));

		if (row.size() > 2) {
			name = QString::fromStdString(std::get<std::string>(row[2]));
		}

		if (row.size() > 3) {
			city = QString::fromStdString(std::get<std::string>(row[3]));
		}

		if (row.size() > 4) {
			state = QString::fromStdString(std::get<std::string>(row[4]));
		}

		if (row.size() > 5) {
			country = QString::fromStdString(std::get<std::string>(row[5]));
		}
		else {
			country = "USA";
		}

		dm->addTeam(key, number, nick, name, city, state, country);
	}

	return false;
}