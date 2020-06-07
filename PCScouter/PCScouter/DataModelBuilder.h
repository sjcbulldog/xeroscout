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
#include "BlueAllianceEngine.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include "ScoutingForm.h"
#include <memory>

class DataModelBuilder
{
public:
	DataModelBuilder() = delete;
	~DataModelBuilder() = delete;

	static void addBlueAllianceData(std::shared_ptr<xero::ba::BlueAlliance> ba, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int maxmatch);

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> 
		buildModel(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingForm> team,
		std::shared_ptr<xero::scouting::datamodel::ScoutingForm> match, const QString& evkey, QString& error);

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
		buildModel(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team,
			std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match, const QString& evkey, QString& error);

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
		buildModel(xero::ba::BlueAllianceEngine& engine, const QString &pitform, const QString &matchform, 
		const QString& evkey, QString& error);

	static bool addTeamsMatches(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);

	static bool addTeams(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QStringList &keys);

	static void jsonToPropMap(const QJsonObject& obj, const QString& alliance, xero::scouting::datamodel::ScoutingDataMapPtr map);

private:
	static void breakoutBlueAlliancePerRobotData(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, 
		std::map<QString, std::pair<xero::scouting::datamodel::ScoutingDataMapPtr, xero::scouting::datamodel::ScoutingDataMapPtr>>& data, int maxmatch);

	static void breakOutBAData(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, xero::scouting::datamodel::Alliance c,
		xero::scouting::datamodel::ScoutingDataMapPtr data);
};

