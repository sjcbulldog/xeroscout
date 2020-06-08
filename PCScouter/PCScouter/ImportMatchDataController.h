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
#include "ScoutingDataMap.h"
#include <QStringList>
#include <map>

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

