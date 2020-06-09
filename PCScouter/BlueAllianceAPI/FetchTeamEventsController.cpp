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
