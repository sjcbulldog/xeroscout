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

#pragma once

#include "blueallianceapi_global.h"
#include "Event.h"
#include "Team.h"
#include "Match.h"
#include "District.h"
#include "BlueAllianceTransport.h"
#include "EngineController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <map>

namespace xero
{
	namespace ba
	{
		class BLUEALLIANCEAPI_EXPORT BlueAllianceEngine
		{
		public:
			BlueAllianceEngine(const QString& server, const QString& authkey);
			virtual ~BlueAllianceEngine();

			bool hasResult();
			std::shared_ptr<BlueAllianceResult> getResult();

			void requestStatus();
			void requestTeams(const std::vector<int>& years);
			void requestTeams(const QStringList& keys);
			void requestEvents(const std::vector<int>& years);
			void requestMatches(const QString& evkey);
			void requestZebra(const QStringList& keys);
			void requestTeamEvents(const QStringList& keys, int year);
			void requestMatchesDetail(const QStringList& keys);
			void requestRankings(const QString& evkey);
			void requestEventTeams(const QString& evkey);

			std::shared_ptr<Team> createTeam(QString key, int num, QString nick, QString name, QString city, QString state, QString country);
			size_t teamCount() const { return teams_by_number_.size(); }
			const std::map<QString, std::shared_ptr<Team>>& teams() { return teams_by_key_; }

			std::shared_ptr<Event> createEvent(QString key, QString name, QString city, QString state, QString country, QString sdate, QString edate, int year);
			size_t eventCount() const { return events_by_key_.size(); }
			const std::map<QString, std::shared_ptr<Event>>& events() { return events_by_key_; }

			std::shared_ptr<District> createDistrict(QString key, QString abbrev, QString name, int year);
			size_t districtCount() const { return districts_by_key_.size(); }
			const std::map<QString, std::shared_ptr<District>>& districts() { return districts_by_key_; }

			std::shared_ptr<Match> createMatch(const QString&evkey, const QString& key, const QString& comp_level, int set_number, int match_number,
				const QString& winning_alliance, int etime, int ptime, int atime, std::shared_ptr<MatchAlliance> red, std::shared_ptr<MatchAlliance> blue);
			size_t matchCount() const { return matches_by_key_.size(); }
			const std::map<QString, std::shared_ptr<Match>>& matches() { return matches_by_key_; }

			bool assignEventToTeam(const QString& team, const QString& evkey);

		private:
			static constexpr const char* IsDataFeedDownKey = "is_datafeed_down";

		private:
			void processStatusResult(std::shared_ptr<QJsonDocument> doc, BlueAllianceResult::Status st, int code);
			void processControllerResult(std::shared_ptr<QJsonDocument> doc, BlueAllianceResult::Status st, int code);

			void addResult(std::shared_ptr<BlueAllianceResult> result);

		private:
			BlueAllianceTransport transport_;
			std::mutex result_lock_;
			std::list<std::shared_ptr<BlueAllianceResult>> results_;
			std::shared_ptr<EngineController> controller_;

			std::map<int, std::shared_ptr<Team>> teams_by_number_;
			std::map<QString, std::shared_ptr<Team>> teams_by_key_;

			std::map<QString, std::shared_ptr<Event>> events_by_key_;

			std::map<QString, std::shared_ptr<District>> districts_by_key_;

			std::map<QString, std::shared_ptr<Match>> matches_by_key_;

			QString error_message_;
		};

	}
}
