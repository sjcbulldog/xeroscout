//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "BlueAlliance.h"
#include "BlueAllianceEngine.h"
#include "BACountResult.h"
#include <QStandardPaths>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <thread>

namespace xero
{
	namespace ba
	{
		BlueAlliance::BlueAlliance()
		{
			engine_state_ = EngineState::Initializing;
			busy_ = false;
			init();
		}

		BlueAlliance::~BlueAlliance()
		{
		}

		bool BlueAlliance::requestEvents(int year)
		{
			std::vector<int> years;

			if (busy_ || engine_state_ != EngineState::Up)
				return false;

			years.push_back(year);
			engine_->requestEvents(years);
			fetch_type_ = "events";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestMatches(const QString& eventkey)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;

			engine_->requestMatches(eventkey);
			fetch_type_ = "match list";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestTeams(const QStringList& teams)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;

			engine_->requestTeams(teams);
			fetch_type_ = "teams";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestMatchesDetails(const QStringList& keys)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;

			engine_->requestMatchesDetail(keys);
			fetch_type_ = "match scoring";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestZebraData(const QStringList& keys)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;

			engine_->requestZebra(keys);
			fetch_type_ = "zebra";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestTeamEvents(const QStringList& keys, int year)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;;

			engine_->requestTeamEvents(keys, year);
			fetch_type_ = "teamevents";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestRankings(const QString& evkey)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;;

			engine_->requestRankings(evkey);
			fetch_type_ = "rankings";
			busy_ = true;

			return true;
		}

		bool BlueAlliance::requestEventTeams(const QString& evkey)
		{
			if (busy_ || engine_state_ != EngineState::Up)
				return false;;

			engine_->requestEventTeams(evkey);
			fetch_type_ = "rankings";
			busy_ = true;

			return true;
		}

		void BlueAlliance::init()
		{
			QString server = "https://www.thebluealliance.com/api/v3";
			QString authkey = "b9OfKDfqHf2Es0e4UAcpCsO4NZ3PUIQWMBHtQ145hhE4jakPJa1pdMVJSPDStcrT";

			engine_ = new BlueAllianceEngine(server, authkey);
			busy_ = true;
			engine_->requestStatus();
		}

		QString BlueAlliance::getStatus()
		{
			QString result;

			switch (engine_state_)
			{
			case EngineState::Down:
				result = QString("BlueAlliance is not reachable");
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

			return result;
		}

		void BlueAlliance::run()
		{
			std::shared_ptr<BlueAllianceResult> result;

			while (1) {
				result = engine_->getResult();
				if (result == nullptr)
					break;

				switch (engine_state_)
				{
				case EngineState::Initializing:
					if (result->status() != BlueAllianceResult::Status::Success) 
					{
						//
						// Error in getting blue alliance status.  This basically makes
						// blue alliance data unavailable.
						//
						engine_state_ = EngineState::Down;
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
						//
						// TODO: this is rare but can happen.  How do we get this up to the
						//       client????
						//
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
