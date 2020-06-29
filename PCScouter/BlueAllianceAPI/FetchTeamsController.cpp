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

#include "FetchTeamsController.h"
#include "BACountResult.h"
#include "BlueAllianceEngine.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <memory>
#include <cassert>

namespace xero
{
	namespace ba
	{

		FetchTeamsController::FetchTeamsController(BlueAllianceEngine& engine, int year) : EngineController(engine)
		{
			year_ = year;
			index_ = 1;
			state_ = State::FetchingKeys;
		}

		FetchTeamsController::FetchTeamsController(BlueAllianceEngine& engine, const QStringList& teams) : EngineController(engine)
		{
			year_ = -1;
			keys_ = teams;
			index_ = 0;
			state_ = State::FetchingData;
			keys_ = teams;

			assert(keys_.size() > 0);
		}

		FetchTeamsController::~FetchTeamsController()
		{
		}

		bool FetchTeamsController::isDone()
		{
			return state_ == State::Complete;
		}

		QString FetchTeamsController::query()
		{
			QString str;

			if (state_ == State::FetchingKeys)
			{
				if (year_ == -1)
				{
					str = "/teams/" + QString::number(index_) + "/keys";
				}
				else
				{
					str = "/teams/" + QString::number(year_) + "/" + QString::number(index_) + "/keys";
				}
			}
			else if (state_ == State::FetchingData)
			{
				str = "/team/" + keys_[index_];
			}

			return str;
		}

		BlueAllianceResult::Status FetchTeamsController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			Q_UNUSED(code);

			if (state_ == State::FetchingKeys)
			{
				if (!doc->isArray())
					return BlueAllianceResult::Status::JSONError;

				QJsonArray arr = doc->array();
				if (arr.size() == 0)
				{
					state_ = State::FetchingData;
					index_ = 0;
				}
				else
				{
					for (int i = 0; i < arr.size(); i++)
					{
						QJsonValue v = arr[i];
						if (v.isString())
						{
							keys_.push_back(v.toString());
						}
					}
					index_++;
				}
			}
			else if (state_ == State::FetchingData)
			{
				if (!doc->isObject())
					return BlueAllianceResult::Status::JSONError;

				QJsonObject obj = doc->object();
				if (!obj.contains("key") || !obj.contains("team_number") || !obj.contains("nickname") || !obj.contains("name") ||
					!obj.contains("school_name") || !obj.contains("city") || !obj.contains("state_prov") || !obj.contains("country") ||
					!obj.contains("address") || !obj.contains("postal_code") || !obj.contains("gmaps_place_id") || !obj.contains("gmaps_url") ||
					!obj.contains("lat") || !obj.contains("lng") || !obj.contains("location_name") || !obj.contains("website") ||
					!obj.contains("rookie_year") || !obj.contains("motto") || !obj.contains("home_championship"))
				{
					return BlueAllianceResult::Status::JSONError;
				}

				QString key = obj["key"].toString();
				int num = obj["team_number"].toInt();
				QString nick = obj["nickname"].toString();
				QString name = obj["name"].toString();
				QString city = obj["city"].toString();
				QString state = obj["state_prov"].toString();
				QString country = obj["country"].toString();
				auto team = engine().createTeam(key, num, nick, name, city, state, country);

				index_++;
				if (index_ == keys_.size())
					state_ = State::Complete;
			}

			return BlueAllianceResult::Status::Success;
		}

		std::shared_ptr<BlueAllianceResult> FetchTeamsController::finishedResult()
		{
			return std::make_shared<BACountResult>((int)engine().teamCount(), (int)engine().teamCount(), true);
		}

		std::shared_ptr<BlueAllianceResult> FetchTeamsController::startResult()
		{
			return std::make_shared<BACountResult>(0, 0, false);
		}

		std::shared_ptr<BlueAllianceResult> FetchTeamsController::progressResult()
		{
			std::shared_ptr<BACountResult> ret;

			if (state_ == State::FetchingData)
				ret = std::make_shared<BACountResult>(index_, (int)keys_.size(), true);
			else
				ret = std::make_shared<BACountResult>(keys_.size(), keys_.size(), false);

			return ret;
		}

	}
}
