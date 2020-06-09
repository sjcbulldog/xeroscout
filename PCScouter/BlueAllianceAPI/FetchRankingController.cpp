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
