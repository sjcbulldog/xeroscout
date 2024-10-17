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
#include "ScoutingDataMap.h"
#include <QStringList>
#include <map>
#include <limits>

//
// Not sure who is defining this, but it is bad
//
#undef max 

class ImportMatchDataController : public ApplicationController
{
public:
	ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		int maxmatch = std::numeric_limits<int>::max());
	virtual ~ImportMatchDataController();

	bool isDone() override;
	void run() override;

private:
	enum class State
	{
		Start,
		Done,
		Error,
		WaitingForMatches,
		WaitingForDetail,
		WaitingForTeams,
		WaitingForRankings
	};

private:
	void breakOutBAData(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, xero::scouting::datamodel::Alliance c, xero::scouting::datamodel::ScoutingDataMapPtr data);
	void breakoutBlueAlliancePerRobotData(std::map<QString, std::pair<xero::scouting::datamodel::ScoutingDataMapPtr, xero::scouting::datamodel::ScoutingDataMapPtr>>& data);
	void gotDetail();
	void gotRankings();

private:
	State state_;
	std::map<QString, QStringList> strings_;
	int maxmatch_;
};

