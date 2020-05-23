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
