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
#include <QString>
#include <QStringList>

namespace xero
{
	namespace ba
	{
		class FetchTeamsController : public EngineController
		{
		public:
			FetchTeamsController(BlueAllianceEngine& engine, int year);
			FetchTeamsController(BlueAllianceEngine& engine, const QStringList& teams);
			virtual ~FetchTeamsController();


			QString name() override {
				return "FetchTeams";
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
				FetchingKeys,
				FetchingData,
				Complete
			};


		private:
			int year_;
			int index_;
			State state_;
			QStringList keys_;
		};

	}
}
