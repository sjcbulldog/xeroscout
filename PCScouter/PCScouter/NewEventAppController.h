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

#pragma once

#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include <QStringList>

class NewEventAppController : public ApplicationController
{
public:
	NewEventAppController(std::shared_ptr<xero::ba::BlueAlliance> ba, const QStringList &tablets, int year);
	virtual ~NewEventAppController();

	bool isDone() override;
	void run() override;

	const QStringList& tablets() {
		return tablets_;
	}

	virtual bool shouldDisableApp();

	void simNoMatches() {
		sim_no_matches_ = true;
	}

private:
	enum class State
	{
		Start,
		WaitingForEvents,
		WaitingForMatches,
		WaitingForTeams,
		NoMatches,
		WaitingForEventTeams,
		WaitingForTeamDetail,
		Done,
		Error,
	};

private:
	void promptUser();
	void start();
	void gotMatches();
	void gotTeams();
	void noMatches();
	void gotEventTeams();
	void gotTeamDetail();

private:	
	int year_;
	State state_;
	QStringList tablets_;
	QStringList team_keys_;
	bool sim_no_matches_;
};
