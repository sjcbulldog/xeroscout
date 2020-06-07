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


#include "FetchMatchController.h"
#include "BACountResult.h"
#include "BlueAllianceEngine.h"
#include "MatchAlliance.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <memory>

namespace xero
{
	namespace ba
	{
		FetchMatchController::FetchMatchController(BlueAllianceEngine& engine, const QStringList& keys) : KeyListEngineController(engine, keys)
		{
		}

		FetchMatchController::~FetchMatchController()
		{
		}

		QString FetchMatchController::query()
		{
			QString str;
			str = "/match/" + getKey();
			return str;
		}

		BlueAllianceResult::Status FetchMatchController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			if (doc->isObject())
			{
				auto matmap = engine().matches();

				QJsonObject obj = doc->object();
				if (!obj.contains("key") || !obj.value("key").isString())
				{
					nextKey();
					return BlueAllianceResult::Status::Success;
				}

				auto it = matmap.find(obj.value("key").toString());
				if (it == matmap.end())
				{
					nextKey();
					return BlueAllianceResult::Status::Success;
				}

				if (obj.contains("score_breakdown") && obj.value("score_breakdown").isObject())
				{
					it->second->setScoreBreakdown(obj.value("score_breakdown").toObject());
				}

				if (obj.contains("videos") && obj.value("videos").isArray())
				{
					QJsonArray videos = obj.value("videos").toArray();
					for (int i = 0; i < videos.size(); i++)
					{
						if (!videos[i].isObject())
							continue;

						QJsonObject obj = videos[i].toObject();

						if (!obj.contains("type") || !obj.value("type").isString())
							continue;

						if (!obj.contains("key") || !obj.value("key").isString())
							continue;

						QString type = obj.value("type").toString();
						QString key = obj.value("key").toString();

						it->second->addVideo(type, key);
					}
				}
			}

			nextKey();
			return BlueAllianceResult::Status::Success;
		}
	}
}
