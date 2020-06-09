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

#include "FetchEventsController.h"
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


		FetchEventsController::FetchEventsController(BlueAllianceEngine& engine, int year) : EngineController(engine)
		{
			year_ = year;
			done_ = false;
		}

		FetchEventsController::~FetchEventsController()
		{
		}

		bool FetchEventsController::isDone()
		{
			return done_;
		}

		QString FetchEventsController::query()
		{
			QString str;

			str = "/events/" + QString::number(year_) + "/simple";

			return str;
		}

		BlueAllianceResult::Status FetchEventsController::processJson(int code, std::shared_ptr<QJsonDocument> doc)
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
				bool bevent_code = obj.contains("event_code");
				bool bevent_type = obj.contains("event_type");
				bool bcity = obj.contains("city");
				bool bstate = obj.contains("state_prov");
				bool bcountry = obj.contains("country");
				bool bsdate = obj.contains("start_date");
				bool bedate = obj.contains("end_date");
				bool bdistrict = obj.contains("district");

				if (!obj.contains("key") || !obj.contains("name") || !obj.contains("event_code") || !obj.contains("event_type") ||
					!obj.contains("city") || !obj.contains("state_prov") || !obj.contains("country") ||
					!obj.contains("start_date") || !obj.contains("end_date") || !obj.contains("year"))
				{
					return BlueAllianceResult::Status::JSONError;
				}

				QString key = obj["key"].toString();
				QString name = obj["name"].toString();
				QString event_code = obj["event_code"].toString();
				int etype = obj["event_type"].toInt();

				QJsonObject dist = obj["district"].toObject();

				QString city = obj["city"].toString();
				QString state = obj["state_prov"].toString();
				QString country = obj["country"].toString();
				QString startdate = obj["start_date"].toString();
				QString enddate = obj["end_date"].toString();
				int year = obj["year"].toInt();

				QString dkey;
				std::shared_ptr<District> district;
				if (obj.contains("district"))
				{
					QJsonObject dobj = obj["district"].toObject();
					QString abbrev = dobj["abberviation"].toString();
					QString dname = dobj["display_name"].toString();
					dkey = dobj["key"].toString();
					int year = dobj["year"].toInt();

					if (dkey.length() > 0)
						district = engine().createDistrict(dkey, abbrev, dname, year);
				}

				auto ev = engine().createEvent(key, name, city, state, country, startdate, enddate, year);
				if (district != nullptr)
				{
					ev->setDistrict(district);
					district->addEvent(ev);
				}

			}

			done_ = true;

			return BlueAllianceResult::Status::Success;
		}

		std::shared_ptr<BlueAllianceResult> FetchEventsController::finishedResult()
		{
			return std::make_shared<BACountResult>((int)engine().eventCount(), (int)engine().eventCount(), true);
		}

		std::shared_ptr<BlueAllianceResult> FetchEventsController::startResult()
		{
			return std::make_shared<BACountResult>(0, 0, false);
		}

		std::shared_ptr<BlueAllianceResult> FetchEventsController::progressResult()
		{
			return std::make_shared<BACountResult>((int)engine().eventCount(), (int)engine().eventCount(), false);
		}

	}
}
