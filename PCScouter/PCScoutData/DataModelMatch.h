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

#include "ScoutingDataMap.h"
#include "Alliance.h"
#include <QJsonObject>
#include <QString>
#include <QVariant>
#include <vector>
#include <memory>
#include <cassert>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class DataModelMatch
			{
				friend class ScoutingDataModel;
				friend class JSonDataModelConverter;

			public:
				DataModelMatch(const QString& key, const QString& comp, int set, int match, int etime) {
					key_ = key;
					comp_type_ = comp;
					set_number_ = set;
					match_number_ = match;
					etime_ = etime;
				}

				virtual ~DataModelMatch() {
				}

				QString title() const {
					QString ret;

					if (comp_type_ == "qm")
						ret = "Qual Match";
					else if (comp_type_ == "qf")
						ret = "Quarter Finals";
					else if (comp_type_ == "sf")
						ret = "Semi Finals";
					else if (comp_type_ == "ef")
						ret = "Einstein Finals";
					else
						ret = "Finals";

					if (comp_type_ != "qm")
						ret += ", Set " + QString::number(set_number_);

					ret += ", Match " + QString::number(match_number_);

					return ret;
				}

				int eventTime() const {
					return etime_;
				}

				bool needsTablet(const QString& tab) const {
					for (auto robot : robots_)
					{
						if (robot->tablet() == tab)
							return true;
					}

					return false;
				}

				const QString& key() const {
					return key_;
				}

				QStringList allTeams() const {
					QStringList ret;

					for (auto robot : robots_)
						ret.push_back(robot->key());

					return ret;
				}

				const QString& compType() const {
					return comp_type_;
				}

				int set() const {
					return set_number_;
				}

				int match() const {
					return match_number_;
				}

				bool hasScoutingData(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->hasScoutingData();
				}

				ConstScoutingDataMapPtr scoutingData(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->scoutingData();
				}

				bool hasExtraData(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->hasExtraData();
				}

				ConstScoutingDataMapPtr extraData(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->extraData();
				}

				bool hasExtraField(Alliance a, const QString &name) const {
					bool ret = false;

					auto robot = findRobotByColorSlot(a, 1);
					assert(robot != nullptr);

					if (robot->hasExtraData())
					{
						auto it = robot->extraData()->find(name);
						ret = (it != robot->extraData()->end());
					}

					return ret;
				}

				QVariant value(Alliance a, int slot, const QString& name) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					QVariant ret;
					ConstScoutingDataMapPtr scout, ba;

					if (robot->hasScoutingData())
					{
						auto it = robot->scoutingData()->find(name);
						if (it != robot->scoutingData()->end())
							ret = it->second;
					}

					if (!ret.isValid() && robot->hasExtraData())
					{
						auto it = robot->extraData()->find(name);
						if (it != robot->extraData()->end())
							ret = it->second;
					}

					return ret;
				}

				ScoutingDataMapPtr data(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					auto data = std::make_shared<ScoutingDataMap>();

					if (robot->hasScoutingData())
					{
						for (auto pair : *robot->scoutingData())
							data->insert_or_assign(pair.first, pair.second);
					}

					if (robot->hasExtraData())
					{
						for (auto pair : *robot->extraData())
							data->insert_or_assign(pair.first, pair.second);
					}
					return data;
				}

				std::list<ConstScoutingDataMapPtr> scoutingDataList(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->scoutingDataList();
				}

				bool tabletToAllianceSlot(const QString& tabname, Alliance& color, int& slot) const {
					color = Alliance::Red;
					slot = -1;

					for (auto robot : robots_)
					{
						if (robot->tablet() == tabname)
						{
							color = robot->color();
							slot = robot->slot();
							return true;
						}
					}
					return false;
				}

				bool teamToAllianceSlot(const QString& teamkey, Alliance& color, int& slot) const {
					color = Alliance::Red;
					slot = -1;

					for (auto robot : robots_)
					{
						if (robot->key() == teamkey)
						{
							color = robot->color();
							slot = robot->slot();
							return true;
						}
					}
					return false;
				}

				const QString& team(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->key();
				}

				const QString& tablet(Alliance a, int slot) const {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->tablet();
				}

				void removeOldScoutingData() {
					for (auto robot : robots_)
						robot->removeOldScoutingData();
				}

				const QJsonObject& zebra() const {
					return zebra_;
				}

			protected:
				void setZebra(const QJsonObject& data) {
					zebra_ = data;
				}

				void setCompType(const QString& ct) {
					comp_type_ = ct;
				}

				void setSet(int n) {
					set_number_ = n;
				}

				void setMatch(int m) {
					match_number_ = m;
				}

				void clearTeams() {
					robots_.clear();
				}

				void addExtraData(Alliance a, int slot, ScoutingDataMapPtr map) {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					robot->addExtraData(map);
				}

				void addTeam(Alliance a, int slot, const QString& key) {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot == nullptr);

					robot = std::make_shared<OneRobot>(a, slot, key);
					robots_.push_back(robot);
				}


				void setTablet(Alliance a, int slot, const QString& tablet) {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					robot->setTablet(tablet);
				}

				void clearScoutingData(Alliance a, int slot)
				{
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					robot->clearScoutingData();
				}

				bool setScoutingData(Alliance a, int slot, ScoutingDataMapPtr data, bool replace = false) {
					auto robot = findRobotByColorSlot(a, slot);
					assert(robot != nullptr);

					return robot->addScoutingData(data, replace);
				}

			private:
				class OneRobot
				{
				public:
					OneRobot(Alliance a, int slot, const QString &key) {
						color_ = a;
						slot_ = slot;
						team_key_ = key;
					}

					Alliance color() const {
						return color_;
					}

					int slot() const {
						return slot_;
					}

					const QString& key() const {
						return team_key_;
					}

					const QString& tablet() const {
						return tablet_;
					}

					void setTablet(const QString& tablet) {
						tablet_ = tablet;
					}

					ConstScoutingDataMapPtr extraData() const {
						return extra_data_;
					}

					bool hasScoutingData() const {
						return scouting_data_.size() > 0;
					}

					bool addScoutingData(ScoutingDataMapPtr data, bool replace) {
						bool ret = false;

						if (replace || scouting_data_.size() == 0)
						{
							scouting_data_.clear();
							scouting_data_.push_back(data);
							ret = true;
						}
						else
						{
							if (*scouting_data_.back() != *data)
							{
								scouting_data_.push_back(data);
								ret = false;
							}
						}

						return ret;
					}

					bool hasExtraData() const {
						return extra_data_ != nullptr;
					}

					void addExtraData(ScoutingDataMapPtr data) {
						if (extra_data_ == nullptr)
							extra_data_ = std::make_shared<ScoutingDataMap>();

						for (auto pair : *data)
						{
							extra_data_->insert_or_assign(pair.first, pair.second);
						}
					}

					ConstScoutingDataMapPtr scoutingData() const {
						if (scouting_data_.size() == 0)
							return nullptr;

						return scouting_data_.back();
					}

					std::list<ConstScoutingDataMapPtr> scoutingDataList() const {
						std::list<ConstScoutingDataMapPtr> ret;

						for (auto ptr : scouting_data_)
							ret.push_back(ptr);

						return ret;
					}

					void removeOldScoutingData() {
						while (scouting_data_.size() > 1)
							scouting_data_.erase(scouting_data_.begin());
					}

					void clearScoutingData() {
						scouting_data_.clear();
					}

				private:
					//
					// The alliance
					//
					Alliance color_;

					//
					// The slot
					//
					int slot_;

					//
					// The team key
					//
					QString team_key_;

					//
					// The tablet used to scout this team in this match
					//
					QString tablet_;

					//
					// The list of scouting data for each team in the red alliance.  Note, there is a list
					// for the case where there are multiple sources of data, but the last set of data is used in all
					// queries.  The merge operation can be used to consolidate the data into a single set.
					//
					std::list<ScoutingDataMapPtr> scouting_data_;

					//
					// The blue alliance data
					//
					ScoutingDataMapPtr extra_data_;
				};

				std::shared_ptr<OneRobot> findRobotByColorSlot(Alliance c, int slot) {
					for (auto robot : robots_)
					{
						if (robot->color() == c && robot->slot() == slot)
							return robot;
					}

					return nullptr;
				}

				std::shared_ptr<const OneRobot> findRobotByColorSlot(Alliance c, int slot) const {
					for (auto robot : robots_)
					{
						if (robot->color() == c && robot->slot() == slot)
							return robot;
					}

					return nullptr;
				}

			private:
				//
				// The blue alliance key for this match
				//
				QString key_;

				//
				// The type of competition, qual match (qm), quarter final (qf), semi final (qf), final (f), or einstein final (ef)
				//
				QString comp_type_;

				//
				// The set number, -1 for qual matches
				//
				int set_number_;

				//
				// The match number
				//
				int match_number_;

				//
				// The event time for the match
				//
				int etime_;

				//
				// The robots in the alliance
				//
				std::vector<std::shared_ptr<OneRobot>> robots_;

				//
				// The zebra data for the match (stored as the JSON from the blue alliance)
				//
				QJsonObject zebra_;
			};
		}
	}
}
