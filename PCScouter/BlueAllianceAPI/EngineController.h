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

#include "BlueAllianceResult.h"
#include <QJsonDocument>

namespace xero
{
	namespace ba
	{
		class BlueAllianceEngine;

		class EngineController
		{
		public:
			EngineController(BlueAllianceEngine& engine) : engine_(engine)
			{
			}

			virtual ~EngineController()
			{
			}

			virtual QString query() = 0;
			virtual BlueAllianceResult::Status processJson(int code, std::shared_ptr<QJsonDocument> doc) = 0;
			virtual BlueAllianceResult::Status processDocNotFound() {
				return BlueAllianceResult::Status::Success;
			}
			virtual bool isDone() = 0;

			virtual std::shared_ptr<BlueAllianceResult> startResult() { return nullptr; }
			virtual std::shared_ptr<BlueAllianceResult> progressResult() { return nullptr; }
			virtual std::shared_ptr<BlueAllianceResult> finishedResult() = 0;


		protected:
			BlueAllianceEngine& engine() { return engine_; }

		private:
			BlueAllianceEngine& engine_;
		};

	}
}
