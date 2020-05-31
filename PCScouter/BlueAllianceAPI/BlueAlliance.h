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

#include "blueallianceapi_global.h"
#include <QString>
#include <vector>

namespace xero
{
	namespace ba
	{

		class BlueAllianceEngine;

		class BLUEALLIANCEAPI_EXPORT BlueAlliance
		{
		public:
			enum class EngineState
			{
				Initializing,
				Up,
				Down
			};

		public:
			BlueAlliance();
			virtual ~BlueAlliance();

			EngineState state() {
				return engine_state_;
			}

			bool isIdle() {
				return engine_state_ == EngineState::Up && !busy_;
			}

			BlueAllianceEngine& getEngine() {
				return *engine_;
			}

			void run();

			QString getStatus();

			bool requestEvents(int year);
			bool requestMatches(const QString& evkey);
			bool requestTeams(const QStringList& teamkeys);
			bool requestMatchesDetails(const QStringList& keys);
			bool requestZebraData(const QStringList& keys);
			bool requestTeamEvents(const QStringList& keys, int year);
			bool requestRankings(const QString& evkey);
			bool requestEventTeams(const QString& evkey);

		private:
			void init();

		private:
			bool busy_;
			EngineState engine_state_;
			BlueAllianceEngine* engine_;
			int loaded_;
			int total_;
			QString fetch_type_;
		};
	}
}
