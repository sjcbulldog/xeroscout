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

#include "pcscoutdata_global.h"
#include "ScoutingDataModel.h"
#include <QObject>
#include <QLibrary>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT DataGenerator : public QObject
			{
				Q_OBJECT

			public:
				DataGenerator(std::shared_ptr<ScoutingDataModel> dm, int year, int maxred, int maxblue, int maxteams);
				virtual ~DataGenerator();

				void run();
				bool isValid() {
					return match_ptr_ != nullptr && team_ptr_ != nullptr;
				}

			signals:
				void logMessage(const QString& msg);

			private:
				void generateTeams();
				void generateMatches();
				void loadGenerator();

				ScoutingDataMapPtr generate(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> team, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);
				std::vector<ScoutingDataMapPtr> generate(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> team, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

			private:
				typedef std::vector<xero::scouting::datamodel::ScoutingDataMapPtr>(*generate_match)(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match,
					std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

				typedef xero::scouting::datamodel::ScoutingDataMapPtr(*generate_team)(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> team,
					std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

			private:
				std::shared_ptr<ScoutingDataModel> dm_;
				int year_;
				int maxred_;
				int maxblue_;
				int maxteam_;
				QLibrary* lib_;

				generate_match match_ptr_;
				generate_team team_ptr_;
			};
		}
	}
}
