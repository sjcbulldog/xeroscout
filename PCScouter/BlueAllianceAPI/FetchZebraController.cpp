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

#include "FetchZebraController.h"
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
		FetchZebraController::FetchZebraController(BlueAllianceEngine& engine, const QStringList& keys) : KeyListEngineController(engine, keys)
		{
		}

		FetchZebraController::~FetchZebraController()
		{
		}

		QString FetchZebraController::query()
		{
			QString str;
			str = "/match/" + getKey() + "/zebra_motionworks";
			return str;
		}

		BlueAllianceResult::Status FetchZebraController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
		{
			Q_UNUSED(code);

			if (doc->isObject())
			{
				QJsonObject obj = doc->object();
				if (obj.contains("key") && obj.value("key").isString() && obj.contains("times") && obj.value("times").isArray() && obj.contains("alliances") && obj.value("alliances").isObject())
				{
					auto matches = engine().matches();
					auto it = matches.find(obj.value("key").toString());
					if (it != matches.end()) {
						it->second->setZebraData(obj);
					}
				}
			}

			nextKey();
			return BlueAllianceResult::Status::Success;
		}
	}
}
