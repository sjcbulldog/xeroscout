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
			virtual QString name() = 0;

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
