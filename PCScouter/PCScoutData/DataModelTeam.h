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
#include <QJsonObject>
#include <QString>
#include <QVariant>
#include <map>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class DataModelTeam
			{
				friend class ScoutingDataModel;
				friend class JSonDataModelConverter;

			public:
				static constexpr const char* RankName = "ba_rank";
				static constexpr const char* OPRName = "calc_opr";
				static constexpr const char* TeamName = "Team";
				static constexpr const char* TeamKeyName = "TeamKey";
				static constexpr const char* TeamNumberName = "TeamNumber";

			public:
				DataModelTeam(const QString& key, int number, const QString& name) {
					key_ = key;
					number_ = number;
					name_ = name;

					addExtraData(TeamName, name);
					addExtraData(TeamNumberName, number);
					addExtraData(TeamKeyName, key);
				}

				virtual ~DataModelTeam() {
				}

				QVariant value(const QString& name) const {
					QVariant ret;

					if (hasScoutingData())
					{
						auto it = team_data_.back()->find(name);
						if (it != team_data_.back()->end())
							ret = it->second;
					}

					if (!ret.isValid() && hasExtraData())
					{
						auto it = extra_data_->find(name);
						if (it != extra_data_->end())
							ret = it->second;
					}

					return ret;
				}

				bool hasValue(const QString& name) const {

					if (hasScoutingData())
					{
						auto it = team_data_.back()->find(name);
						if (it != team_data_.back()->end())
							return true;
					}

					if (hasExtraData())
					{
						auto it = extra_data_->find(name);
						if (it != extra_data_->end())
							return true;
					}

					return false;
				}

				bool hasRank() const {
					auto it = extra_data_->find(RankName);
					return it != extra_data_->end();
				}

				int rank() const {
					auto it = extra_data_->find(RankName);
					assert(it != extra_data_->end());
					assert(it->second.type() == QVariant::Int);
					return it->second.toInt();
				}

				bool hasRankingInfo() const {
					return !ranking_.isEmpty();
				}

				const QJsonObject& rankingInfo() const {
					return ranking_;
				}

				const QString& key() const {
					return key_;
				}

				int number() const {
					return number_;
				}

				const QString& name() const {
					return name_;
				}

				bool hasScoutingData() const {
					return team_data_.size() > 0;
				}

				ConstScoutingDataMapPtr teamScoutingData() const {
					if (team_data_.size() == 0)
						return nullptr;

					return team_data_.back();
				}

				bool hasExtraData() const {
					return extra_data_ != nullptr;
				}

				ConstScoutingDataMapPtr extraData() const {
					return extra_data_;
				}

				std::list<ConstScoutingDataMapPtr> teamScoutingDataList() const {
					std::list<ConstScoutingDataMapPtr> ret;

					for (auto p : team_data_)
						ret.push_back(p);

					return ret;
				}

				const QString& tablet() const {
					return tablet_;
				}

				void removeOldScoutingData() {
					while (team_data_.size() > 1)
						team_data_.erase(team_data_.begin());
				}

			protected:
				void addExtraData(ScoutingDataMapPtr data) {
					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();

					for (auto pair : *data)
						extra_data_->insert_or_assign(pair.first, pair.second);
				}

				void addExtraData(const QString& name, const QVariant& v) {
					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();

					extra_data_->insert_or_assign(name, v);
				}

				void setOPR(double opr) {
					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();
					extra_data_->insert_or_assign(OPRName, opr);
				}

				void setRanking(const QJsonObject& obj) {
					ranking_ = obj;

					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();

					extra_data_->insert_or_assign(RankName, obj["rank"].toInt());
				}

				void setNumber(int n) {
					number_ = n;
				}

				void setName(const QString& name) {
					name_ = name;
				}

				void clearPitScoutingData() {
					team_data_.clear();
				}

				bool setTeamScoutingData(ScoutingDataMapPtr d, bool replace) {
					assert(d != nullptr);

					if (replace)
					{
						team_data_.clear();
					}
					else
					{
						for (auto pd : team_data_)
						{
							if (compareTwoMaps(pd, d))
								return false;
						}
					}

					team_data_.push_back(d);
					return true;
				}

				void setTablet(const QString& tablet) {
					tablet_ = tablet;
				}

			private:
				QString key_;
				int number_;
				QString name_;
				QString tablet_;
				std::list<ScoutingDataMapPtr> team_data_;
				QJsonObject ranking_;
				ScoutingDataMapPtr extra_data_;
			};
		}
	}
}
