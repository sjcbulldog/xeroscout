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

#include "FetchTeamEventsController.h"
#include "BACountResult.h"
#include "BlueAllianceEngine.h"
#include "MatchAlliance.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

namespace xero
{
	namespace ba
	{
		FetchTeamEventsController::FetchTeamEventsController(BlueAllianceEngine& engine, const QStringList& keys, int year) : KeyListEngineController(engine, keys)
		{
			year_ = year;
		}

		FetchTeamEventsController::~FetchTeamEventsController()
		{
		}

		QString FetchTeamEventsController::query()
		{
			QString str;
			str = "/team/" + getKey() + "/events/" + QString::number(year_) + "/simple";
			return str;
		}

		BlueAllianceResult::Status FetchTeamEventsController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			if (doc->isArray())
			{
				QJsonArray array = doc->array();
				for (int i = 0; i < array.size(); i++)
				{
					if (array[i].isObject())
					{
						const QJsonObject& obj = array[i].toObject();
						if (obj.contains("key") && obj.value("key").isString())
						{
							engine().assignEventToTeam(getKey(), obj.value("key").toString());
						}
					}
				}
			}

			nextKey();
			return BlueAllianceResult::Status::Success;
		}
	}
}
