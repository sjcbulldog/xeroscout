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

#include "FetchRankingController.h"
#include "BACountResult.h"
#include "BlueAllianceEngine.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <memory>

namespace xero
{
	namespace ba
	{
		FetchRankingController::FetchRankingController(BlueAllianceEngine& engine, const QString &evkey) : EngineController(engine)
		{
			evkey_ = evkey;
			done_ = false;
		}

		FetchRankingController::~FetchRankingController()
		{
		}

		bool FetchRankingController::isDone()
		{
			return done_;
		}

		QString FetchRankingController::query()
		{
			QString str;

			str = "/event/" + evkey_ + "/rankings";

			return str;
		}

		BlueAllianceResult::Status FetchRankingController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			QJsonObject obj = doc->object();

			if (obj.contains("rankings") && obj.value("rankings").isArray())
			{
				QJsonArray array = obj.value("rankings").toArray();
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject rankobj = array[i].toObject();
					if (!rankobj.contains("team_key") || !rankobj.value("team_key").isString())
						continue;

					auto it = engine().teams().find(rankobj.value("team_key").toString());
					if (it == engine().teams().end())
						continue;

					it->second->setRanking(rankobj);
				}
			}

			done_ = true;
			return BlueAllianceResult::Status::Success;
		}

		std::shared_ptr<BlueAllianceResult> FetchRankingController::finishedResult()
		{
			return std::make_shared<BACountResult>(1, 1, true);
		}

		std::shared_ptr<BlueAllianceResult> FetchRankingController::startResult()
		{
			return std::make_shared<BACountResult>(0, 1, false);
		}
	}
}
