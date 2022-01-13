#include "NewOffseasonEventAppController.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

NewOffseasonEventAppController::NewOffseasonEventAppController(ImageManager& mgr, const QStringList& tablets, int year) : ApplicationController(nullptr, nullptr), images_(mgr)
{
	tablets_ = tablets;
	state_ = State::Start;
	year_ = year;
	sim_no_matches_ = false;
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
}