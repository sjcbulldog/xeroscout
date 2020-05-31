#include "FetchEventTeamsController.h"
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
		FetchEventTeamsController::FetchEventTeamsController(BlueAllianceEngine& engine, const QString& evkey) : EngineController(engine)
		{
			evkey_ = evkey;
			done_ = false;
		}

		FetchEventTeamsController::~FetchEventTeamsController()
		{
		}

		bool FetchEventTeamsController::isDone()
		{
			return done_;
		}

		QString FetchEventTeamsController::query()
		{
			QString str;

			str = "/event/" + evkey_ + "/teams/simple";

			return str;
		}

		BlueAllianceResult::Status FetchEventTeamsController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			QJsonArray array = doc->array();

			std::shared_ptr<Event> ev;
			auto it = engine().events().find(evkey_);
			if (it != engine().events().end())
				ev = it->second;

			for (int i = 0; i < array.size(); i++)
			{
				if (!array[i].isObject())
					continue;

				QJsonObject obj = array[i].toObject();

				if (!obj.contains("key") || !obj.value("key").isString())
					continue;
				QString key = obj.value("key").toString();

				if (!obj.contains("team_number") || !obj.value("team_number").isDouble())
					continue;
				int num = obj.value("team_number").toInt();

				if (!obj.contains("nickname") || !obj.value("nickname").isString())
					continue;
				QString nick = obj.value("nickname").toString();

				if (!obj.contains("name") || !obj.value("name").isString())
					continue;
				QString name = obj.value("name").toString();

				if (!obj.contains("city") || !obj.value("city").isString())
					continue;
				QString city = obj.value("city").toString();

				if (!obj.contains("state_prov") || !obj.value("state_prov").isString())
					continue;
				QString state = obj.value("state_prov").toString();

				if (!obj.contains("country") || !obj.value("country").isString())
					continue;
				QString country = obj.value("country").toString();

				engine().createTeam(key, num, nick, name, city, state, country);

				if (ev != nullptr)
					ev->addTeam(key);
			}


			done_ = true;
			return BlueAllianceResult::Status::Success;
		}

		std::shared_ptr<BlueAllianceResult> FetchEventTeamsController::finishedResult()
		{
			return std::make_shared<BACountResult>(1, 1, true);
		}

		std::shared_ptr<BlueAllianceResult> FetchEventTeamsController::startResult()
		{
			return std::make_shared<BACountResult>(0, 1, false);
		}
	}
}
