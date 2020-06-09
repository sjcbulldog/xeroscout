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

#include "BlueAlliance.h"
#include "Event.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include "ApplicationController.h"
#include <QJsonObject>
#include <QStringList>
#include <QString>
#include <QElapsedTimer>
#include <memory>

class KPIController : public ApplicationController
{
public:
	KPIController(std::shared_ptr<xero::ba::BlueAlliance> ba, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QDate &evdate, const QStringList& teams,
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
		WaitingOnTeamsPhase2,
		Error,
	};

private:
	void getEvents();
	void getTeams();
	void getTeamEvents();

	void gotTeamEvents();
	void gotMatches();
	void gotMatchDetail();

	void computeKPI();

	xero::scouting::datamodel::ScoutingDataMapPtr evToData(const QString& tkey, const QString& evkey);

private:
	QDate evdate_;
	State state_;
	QStringList teams_;
	QStringList evlist_;
	QString evkey_;
	int index_ ;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team_;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match_;
	std::map<QString, QStringList> team_event_map_;
	std::map<QString, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>> models_;
	QElapsedTimer timer_;
};

