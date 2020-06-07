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
