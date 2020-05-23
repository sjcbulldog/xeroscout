
//
// Copyright 2020 by Jack W. (Butch) Griffin
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
#include "TeamDataSummary.h"
#include "ScoutingDataSet.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class ScoutingDataModel;

			class PCSCOUTDATA_EXPORT AllTeamSummaryGenerator : public QObject
			{
				Q_OBJECT

			public:
				AllTeamSummaryGenerator(std::shared_ptr<ScoutingDataModel> dm, ScoutingDataSet& ds);
				virtual ~AllTeamSummaryGenerator();

				void start();
				bool run();
				bool done() { return state_ == State::Complete; }
				bool hasError() {
					return error_.length() > 0;
				}

				const QString& error() {
					return error_;
				}

			signals:
				void reportProgress(int pcnt);

			private:
				void createChoicesSet(const std::list<std::shared_ptr<TeamDataSummary>>& list, std::map<QString, QStringList>& choices);
				void generateDataSet();

			private:
				enum class State {
					Start,
					TeamSummaries,
					CreateChoicesSet,
					CreateDataSet,
					Complete
				};

			private:
				ScoutingDataSet& ds_;
				QString error_;
				std::map<QString, QStringList> choices_;
				std::shared_ptr<ScoutingDataModel> dm_;
				State state_;
				int index_;
				std::list<std::shared_ptr<TeamDataSummary>> summaries_;
				QStringList keys_;
			};

		}
	}
}
