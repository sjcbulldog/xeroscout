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
			Q_UNUSED(code);

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

						QJsonObject vobj = videos[i].toObject();

						if (!vobj.contains("type") || !vobj.value("type").isString())
							continue;

						if (!vobj.contains("key") || !vobj.value("key").isString())
							continue;

						QString type = vobj.value("type").toString();
						QString key = vobj.value("key").toString();

						it->second->addVideo(type, key);
					}
				}
			}

			nextKey();
			return BlueAllianceResult::Status::Success;
		}
	}
}
