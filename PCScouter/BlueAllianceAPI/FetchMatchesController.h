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
#include "EngineController.h"

namespace xero
{
	namespace ba
	{
		class MatchAlliance;

		class FetchMatchesController : public EngineController
		{
		public:
			FetchMatchesController(BlueAllianceEngine& engine, const QString& evid);
			virtual ~FetchMatchesController();


			QString name() override {
				return "FetchMatches";
			}


			virtual QString query();
			virtual BlueAllianceResult::Status processJson(int code, std::shared_ptr<QJsonDocument> doc);
			virtual bool isDone();

			virtual std::shared_ptr<BlueAllianceResult> finishedResult();
			virtual std::shared_ptr<BlueAllianceResult> startResult();
			virtual std::shared_ptr<BlueAllianceResult> progressResult();

		private:
			enum class State
			{
				ReadingMatches,
				Complete
			};

			std::shared_ptr<MatchAlliance> parseAlliance(QJsonObject obj);


		private:
			QString evid_;
			State state_;
		};

	}
}
