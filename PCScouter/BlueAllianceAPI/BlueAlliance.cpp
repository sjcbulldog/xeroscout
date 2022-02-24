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

#include "BlueAlliance.h"
#include "BlueAllianceEngine.h"
#include "BACountResult.h"
#include <QStandardPaths>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <thread>
#include <fstream>

namespace xero
{
	namespace ba
	{
		BlueAlliance::BlueAlliance()
		{
			engine_state_ = EngineState::Initializing;
			busy_ = false;
			logfile_ = nullptr;
			QString logfiledir = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppConfigLocation).first();
			QDir dir(logfiledir);
			if (dir.mkpath("."))
			{
				QString logfile = logfiledir + "/log.txt";
				std::ofstream *strm = new std::ofstream(logfile.toStdString());
				if (!strm->is_open())
				{
					strm = nullptr;
				}

				logfile_ = strm;
			}
			init();
		}

		BlueAlliance::~BlueAlliance()
		{
		}

		void BlueAlliance::bringUp()
		{
			engine_state_ = EngineState::Initializing;
			init();
		}

		bool BlueAlliance::requestEvents(int year)
		{
			std::vector<int> years;

			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;

			years.push_back(year);
			engine_->requestEvents(years);
			fetch_type_ = "events";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestMatches(const QString& eventkey)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;

			engine_->requestMatches(eventkey);
			fetch_type_ = "match list";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestTeams(const QStringList& teams)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;

			engine_->requestTeams(teams);
			fetch_type_ = "teams";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestMatchesDetails(const QStringList& keys)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;

			engine_->requestMatchesDetail(keys);
			fetch_type_ = "match scoring";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestZebraData(const QStringList& keys)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;

			engine_->requestZebra(keys);
			fetch_type_ = "zebra";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestTeamEvents(const QStringList& keys, int year)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;;

			engine_->requestTeamEvents(keys, year);
			fetch_type_ = "teamevents";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestRankings(const QString& evkey)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;;

			engine_->requestRankings(evkey);
			fetch_type_ = "rankings";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestEventTeams(const QString& evkey)
		{
			if (busy_ || engine_state_ != EngineState::Up || engine_ == nullptr)
				return false;;

			engine_->requestEventTeams(evkey);
			fetch_type_ = "rankings";
			busy_ = true;

			return true;
		}

		void BlueAlliance::init()
		{
			QString server = "https://www.thebluealliance.com/api/v3";
			QString authkey = "cgbzLmpXlA5GhIew3E4xswwLqHOm4j0hQ1Mizvg71zkuQZIazcXgf3dd8fguhpxC";

			log(QString("Initializing the build alliance API - key ") + authkey);

			engine_ = new BlueAllianceEngine(logfile_, server, authkey);
			busy_ = true;
			engine_->requestStatus();
		}

		QString BlueAlliance::getStatus()
		{
			QString result;

			if (engine_ == nullptr)
			{
				result = QString("BlueAlliance is down: ") + BlueAllianceResult::toString(status_);
			}
			else
			{
				switch (engine_state_)
				{
				case EngineState::Down:
					result = QString("BlueAlliance is down: ") + BlueAllianceResult::toString(status_);
					break;

				case EngineState::Initializing:
					result = "Connecting to Blue Alliance ...";
					break;

				case EngineState::Up:
					if (busy_)
					{
						result = "BlueAlliance: loading '" + fetch_type_ + "' (" + QString::number(loaded_) + " of " + QString::number(total_) + ")";
					}
					else
					{
						result = "BlueAlliance: ";
						result += QString::number(engine_->eventCount()) + " events";
						result += ", " + QString::number(engine_->matchCount()) + " matches";
						result += ", " + QString::number(engine_->teamCount()) + " teams";
					}
					break;
				}
			}

			return result;
		}

		void BlueAlliance::run()
		{
			std::shared_ptr<BlueAllianceResult> result;

			while (1) {
				if (engine_ == nullptr)
					break;

				result = engine_->getResult();
				if (result == nullptr)
					break;

				switch (engine_state_)
				{
				case EngineState::Initializing:
					if (result->status() != BlueAllianceResult::Status::Success) 
					{
						status_ = result->status();

						//
						// Error in getting blue alliance status.  This basically makes
						// blue alliance data unavailable.
						//
						engine_state_ = EngineState::Down;
						delete engine_;
						engine_ = nullptr;
					}
					else 
					{
						// Mark the system as up
						engine_state_ = EngineState::Up;
					}
					busy_ = false;
					break;

				case EngineState::Down:
					busy_ = false;
					break;

				case EngineState::Up:
					if (result->status() != BlueAllianceResult::Status::Success) 
					{
						engine_state_ = EngineState::Down;
						busy_ = false;
					}
					else 
					{
						auto cntresult = std::dynamic_pointer_cast<BACountResult>(result);
						if (cntresult != nullptr) {
							if (cntresult->isDone()) {
								busy_ = false;
							}
							else {
								loaded_ = cntresult->count();
								total_ = cntresult->total();
							}
						}
					}
					break;
				}
			}
		}
	}
}
