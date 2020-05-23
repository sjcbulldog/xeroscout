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

#include "BlueAllianceEngine.h"
#include "BACountResult.h"
#include "FetchEventsController.h"
#include "FetchTeamsController.h"
#include "FetchMatchesController.h"
#include "FetchMatchController.h"
#include "FetchZebraController.h"
#include "FetchTeamEventsController.h"
#include "FetchRankingController.h"
#include <QDebug>
#include <functional>

namespace xero
{
	namespace ba
	{
		BlueAllianceEngine::BlueAllianceEngine(const QString& server, const QString& authkey) : transport_(server, authkey)
		{
		}

		BlueAllianceEngine::~BlueAllianceEngine()
		{
		}

		bool BlueAllianceEngine::assignEventToTeam(const QString& team, const QString& evkey)
		{
			auto it = teams_by_key_.find(team);
			if (it == teams_by_key_.end())
				return false;

			it->second->addEvent(evkey);
			return true;
		}

		void BlueAllianceEngine::addResult(std::shared_ptr<BlueAllianceResult> result)
		{
			std::lock_guard<std::mutex> lock(result_lock_);
			results_.push_back(result);
		}

		bool BlueAllianceEngine::hasResult()
		{
			std::lock_guard<std::mutex> lock(result_lock_);
			return results_.size() > 0;
		}

		std::shared_ptr<BlueAllianceResult> BlueAllianceEngine::getResult()
		{
			std::shared_ptr<BlueAllianceResult> ret;

			std::lock_guard<std::mutex> lock(result_lock_);
			if (results_.size() > 0)
			{
				ret = results_.front();
				results_.pop_front();
			}

			return ret;
		}

		std::shared_ptr<Team> BlueAllianceEngine::createTeam(QString key, int num, QString nick, QString name, QString city, QString state, QString country)
		{
			std::shared_ptr<Team> ret;

			auto it = teams_by_key_.find(key);
			if (it == teams_by_key_.end())
			{
				ret = std::make_shared<Team>(key, num, nick, name, city, state, country);
				teams_by_key_.insert(std::make_pair(key, ret));
				teams_by_number_.insert(std::make_pair(num, ret));
				qDebug() << "Created team " << nick;
			}
			else
			{
				ret = it->second;
			}

			return ret;
		}

		std::shared_ptr<Event> BlueAllianceEngine::createEvent(QString key, QString name, QString city, QString state, QString country, QString sdate, QString edate, int year)
		{
			std::shared_ptr<Event> ret;

			auto it = events_by_key_.find(key);
			if (it == events_by_key_.end())
			{
				ret = std::make_shared<Event>(key, name, city, state, country, sdate, edate, year);
				events_by_key_.insert(std::make_pair(key, ret));
				qDebug() << "Created event " << name;
			}
			else
			{
				ret = it->second;
			}

			return ret;
		}

		std::shared_ptr<District> BlueAllianceEngine::createDistrict(QString key, QString abbrev, QString name, int year)
		{
			std::shared_ptr<District> ret;

			auto it = districts_by_key_.find(key);
			if (it == districts_by_key_.end())
			{
				ret = std::make_shared<District>(key, abbrev, name, year);
				districts_by_key_.insert(std::make_pair(key, ret));
				qDebug() << "Created district " << abbrev << " " << name << " " << key << " " << year;
			}
			else
			{
				ret = it->second;
			}

			return ret;
		}

		std::shared_ptr<Match> BlueAllianceEngine::createMatch(const QString &evkey, const QString& key, const QString& comp_level, int set_number, int match_number,
			const QString& winning_alliance, int etime, int ptime, int atime, std::shared_ptr<MatchAlliance> red, std::shared_ptr<MatchAlliance> blue)
		{
			std::shared_ptr<Match> ret;

			auto it = matches_by_key_.find(key);
			if (it == matches_by_key_.end()) {
				ret = std::make_shared<Match>(key, comp_level, set_number, match_number, winning_alliance, etime, ptime, atime, red, blue);
				matches_by_key_.insert(std::make_pair(key, ret));
				qDebug() << "Created match " << key << " " << comp_level << " " << set_number << " " << match_number << " " << winning_alliance << " " << etime << " " << ptime << " " << atime << " ";
			}
			else {
				ret = it->second;
			}

			auto evit = events_by_key_.find(evkey);
			if (evit != events_by_key_.end())
			{
				evit->second->addMatch(ret);
			}

			return ret;
		}

		void BlueAllianceEngine::processStatusResult(std::shared_ptr<QJsonDocument> doc, BlueAllianceResult::Status st, int code)
		{
			bool err = false;

			if (st != BlueAllianceResult::Status::Success || code != 200)
			{
				std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(st);
				addResult(result);
				return;
			}

			if (doc->isObject())
			{
				QJsonObject obj = doc->object();
				if (obj.contains(IsDataFeedDownKey))
				{
					QJsonValue v = obj[IsDataFeedDownKey];
					if (v.isBool() && v.toBool() == true)
					{
						std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(BlueAllianceResult::Status::SiteDown);
						addResult(result);
					}
					else
					{
						std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(BlueAllianceResult::Status::Success);
						addResult(result);
					}
				}
			}
			else
			{
				err = true;
			}

			if (err)
			{
				std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(BlueAllianceResult::Status::JSONError);
				addResult(result);
			}
		}

		void BlueAllianceEngine::requestStatus()
		{
			auto cb = std::bind(&BlueAllianceEngine::processStatusResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			if (!transport_.request("/status", cb))
			{
				std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(BlueAllianceResult::Status::ConnectionError);
				addResult(result);
			}
		}

		void BlueAllianceEngine::processControllerResult(std::shared_ptr<QJsonDocument> doc, BlueAllianceResult::Status st, int code)
		{
			bool err = false;
			std::shared_ptr<BlueAllianceResult> result;

			if (st != BlueAllianceResult::Status::Success && code != 404)
			{
				qDebug() << "TRANSPORT ERROR";
				std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(st);
				addResult(result);
				controller_ = nullptr;
				return;
			}

			if (code == 404)
			{
				st = controller_->processDocNotFound();
				if (st != BlueAllianceResult::Status::Success)
				{
					std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(st);
					addResult(result);
					controller_ = nullptr;
					return;
				}
			}
			else if (doc != nullptr)
			{
				st = controller_->processJson(code, doc);
				if (st != BlueAllianceResult::Status::Success)
				{
					std::shared_ptr<BlueAllianceResult> result = std::make_shared<BlueAllianceResult>(st);
					addResult(result);
					controller_ = nullptr;
					return;
				}
			}

			if (controller_->isDone())
			{
				std::shared_ptr<BlueAllianceResult> result = controller_->finishedResult();
				controller_ = nullptr;
				addResult(result);
				return;
			}


			result = controller_->progressResult();
			if (result != nullptr)
				addResult(result);

			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			if (!transport_.request(controller_->query(), cb))
			{
				controller_ = nullptr;
				result = std::make_shared<BlueAllianceResult>(BlueAllianceResult::Status::ConnectionError);
				addResult(result);
			}
		}

		void BlueAllianceEngine::requestTeams(const std::vector<int>& years)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchTeamsController>(*this, years.front());
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestTeams(const QStringList& teams)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchTeamsController>(*this, teams);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestEvents(const std::vector<int>& years)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchEventsController>(*this, years.front());
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestMatches(const QString& evid)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchMatchesController>(*this, evid);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestZebra(const QStringList& keys)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchZebraController>(*this, keys);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestTeamEvents(const QStringList& keys, int year)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchTeamEventsController>(*this, keys, year);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestMatchesDetail(const QStringList& keys)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchMatchController>(*this, keys);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}

		void BlueAllianceEngine::requestRankings(const QString& evkey)
		{
			auto cb = std::bind(&BlueAllianceEngine::processControllerResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			controller_ = std::make_shared<FetchRankingController>(*this, evkey);
			auto result = controller_->startResult();
			if (result != nullptr)
				addResult(result);

			processControllerResult(nullptr, BlueAllianceResult::Status::Success, 200);
		}
	}
}
