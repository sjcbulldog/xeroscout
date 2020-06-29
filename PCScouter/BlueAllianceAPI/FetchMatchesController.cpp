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

#include "FetchMatchesController.h"
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


		FetchMatchesController::FetchMatchesController(BlueAllianceEngine& engine, const QString& evid) : EngineController(engine)
		{
			evid_ = evid;
			state_ = State::ReadingMatches;
		}

		FetchMatchesController::~FetchMatchesController()
		{
		}

		bool FetchMatchesController::isDone()
		{
			return state_ == State::Complete;
		}

		QString FetchMatchesController::query()
		{
			QString str;

			if (state_ == State::ReadingMatches)
			{
				str = "/event/" + evid_ + "/matches/simple";
			}

			return str;
		}

		std::shared_ptr<MatchAlliance> FetchMatchesController::parseAlliance(QJsonObject obj)
		{
			if (!obj.contains("team_keys") || !obj.value("team_keys").isArray())
				return nullptr;

			std::shared_ptr<MatchAlliance> alliance = std::make_shared<MatchAlliance>();

			QJsonArray arr = obj.value("team_keys").toArray();
			for (int i = 0; i < arr.size(); i++) {
				if (!arr[i].isString())
					continue;

				alliance->addTeam(arr[i].toString());
			}

			if (obj.contains("surrogate_team_keys") && obj.value("surrogate_team_keys").isArray())
			{
				QJsonArray sarr = obj.value("surrogate_team_keys").toArray();
				for (int i = 0; i < sarr.size(); i++) {
					if (!sarr[i].isString())
						continue;

					alliance->addSurrogate(sarr[i].toString());
				}
			}

			if (obj.contains("dq_team_keys") && obj.value("dq_team_keys").isArray())
			{
				QJsonArray darr = obj.value("dq_team_keys").toArray();
				for (int i = 0; i < darr.size(); i++) {
					if (!darr[i].isString())
						continue;

					alliance->addDQ(darr[i].toString());
				}
			}

			return alliance;
		}

		BlueAllianceResult::Status FetchMatchesController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			Q_UNUSED(code);

			if (state_ == State::ReadingMatches)
			{
				if (!doc->isArray())
					return BlueAllianceResult::Status::JSONError;

				QJsonArray arr = doc->array();
				for (int i = 0; i < arr.size(); i++)
				{
					QJsonValue v = arr[i];
					if (!v.isObject())
						return BlueAllianceResult::Status::JSONError;

					QJsonObject obj = v.toObject();

					bool bkey = obj.contains("key");
					bool bcomp_level = obj.contains("comp_level");
					bool bset_number = obj.contains("set_number");
					bool bmatch_number = obj.contains("match_number");
					bool balliances = obj.contains("alliances");
					bool bwinning_alliance = obj.contains("winning_alliance");
					bool bevent_key = obj.contains("event_key");
					bool btime = obj.contains("time");
					bool bpredicted_time = obj.contains("predicted_time");
					bool bactual_time = obj.contains("actual_time");

					if (!bkey || !bcomp_level || !bset_number || !bmatch_number || !balliances || !bwinning_alliance || !bevent_key || !btime || !bpredicted_time || !bactual_time)
						return BlueAllianceResult::Status::JSONError;

					QString key = obj["key"].toString();
					QString comp_level = obj["comp_level"].toString();
					int set_number = obj["set_number"].toInt();
					int match_number = obj["match_number"].toInt();

					QJsonObject alliances = obj["alliances"].toObject();

					QString winning_alliance = obj["winning_alliance"].toString();
					QString event_key = obj["event_key"].toString();
					int etime = obj["time"].toInt();
					int ptime = obj["predicted_time"].toInt();
					int atime = obj["actual_time"].toInt();

					std::shared_ptr<MatchAlliance> red, blue;

					QString dkey;
					if (obj.contains("alliances"))
					{
						QJsonObject dobj = obj["alliances"].toObject();

						if (dobj.contains("red") && dobj.value("red").isObject())
							red = parseAlliance(dobj.value("red").toObject());

						if (dobj.contains("blue") && dobj.value("blue").isObject())
							blue = parseAlliance(dobj.value("blue").toObject());
					}
					engine().createMatch(evid_, key, comp_level, set_number, match_number, etime, ptime, atime, red, blue);
				}

				state_ = State::Complete;
			}

			return BlueAllianceResult::Status::Success;
		}

		std::shared_ptr<BlueAllianceResult> FetchMatchesController::finishedResult()
		{
			return std::make_shared<BACountResult>((int)engine().eventCount(), (int)engine().eventCount(), true);
		}

		std::shared_ptr<BlueAllianceResult> FetchMatchesController::startResult()
		{
			return std::make_shared<BACountResult>(0, 0, false);
		}

		std::shared_ptr<BlueAllianceResult> FetchMatchesController::progressResult()
		{
			return std::make_shared<BACountResult>((int)engine().eventCount(), (int)engine().eventCount(), false);
		}

	}
}
