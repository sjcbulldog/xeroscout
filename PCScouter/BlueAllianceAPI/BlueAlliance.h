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

#include "blueallianceapi_global.h"
#include "BlueAllianceResult.h"
#include <QString>
#include <vector>
#include <iostream>

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

			void bringUp();

			bool isIdle() {
				return engine_state_ == EngineState::Up && !busy_;
			}

			BlueAllianceEngine& getEngine() {
				return *engine_;
			}

			void run();

			void log(const QString& line) {
				if (logfile_ != nullptr)
				{
					(*logfile_) << line.toStdString() << std::endl;
				}
			}

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
			std::ostream* logfile_;
			bool busy_;
			EngineState engine_state_;
			BlueAllianceEngine* engine_;
			int loaded_;
			int total_;
			QString fetch_type_;
			BlueAllianceResult::Status status_;
		};
	}
}
