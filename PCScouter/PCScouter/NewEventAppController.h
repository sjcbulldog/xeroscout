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
