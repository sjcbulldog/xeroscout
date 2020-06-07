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

#include "BlueAlliance.h"
#include "Event.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include "ApplicationController.h"
#include <QJsonObject>
#include <QStringList>
#include <QString>
#include <memory>

class KPIController : public ApplicationController
{
public:
	KPIController(std::shared_ptr<xero::ba::BlueAlliance> ba, const QDate &evdate, const QStringList& teams,
		const QString& evkey, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team,
		std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match);
	virtual ~KPIController();

	void run();

	bool isDone() {
		return state_ == State::Done;
	}

private:
	enum class State
	{
		Done,
		Start,
		WaitingOnEvents,
		WaitingOnTeams,
		WaitingOnTeamEvents,
		WaitingOnMatches,
		WaitingOnMatchDetail,
		Error,
	};

private:
	void getEvents();
	void getTeams();
	void getTeamEvents();

	void gotTeamEvents();
	void gotMatches();

	void computeKPI();
private:
	QDate evdate_;
	State state_;
	QStringList teams_;
	QStringList evlist_;
	QString evkey_;
	int index_ ;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team_;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match_;
};

