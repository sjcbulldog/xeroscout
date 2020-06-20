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
				static constexpr const char* DPRName = "calc_dpr";
				static constexpr const char* NPRName = "calc_npr";
				static constexpr const char* TeamName = "Team";
				static constexpr const char* TeamKeyName = "TeamKey";
				static constexpr const char* TeamNumberName = "TeamNumber";
				static constexpr const char* TeamNickName = "NickName";
				static constexpr const char* TeamCityName = "City";
				static constexpr const char* TeamStateName = "State";
				static constexpr const char* TeamCountryName = "Country";

			public:
				DataModelTeam(const QString& key, int number, const QString &nick, const QString& name, const QString &city, const QString &state, const QString &country) {
					key_ = key;
					number_ = number;
					name_ = name;
					nick_ = nick;
					city_ = city;
					state_ = state;
					country_ = country;

					addExtraData(TeamName, name);
					addExtraData(TeamNumberName, number);
					addExtraData(TeamKeyName, key);
					addExtraData(TeamNickName, nick);
					addExtraData(TeamCityName, city);
					addExtraData(TeamStateName, state);
					addExtraData(TeamCountryName, country);
				}

				virtual ~DataModelTeam() {
				}

				QString title() const {
					return QString::number(number_) + " - " + nick_;
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

				const QString& nick() const {
					return nick_;
				}

				const QString& city() const {
					return city_;
				}

				const QString& state() const {
					return state_;
				}

				const QString& country() const {
					return country_;
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

				void setOPR(double opr) {
					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();
					extra_data_->insert_or_assign(OPRName, opr);
				}

				void setDPR(double opr) {
					if (extra_data_ == nullptr)
						extra_data_ = std::make_shared<ScoutingDataMap>();
					extra_data_->insert_or_assign(DPRName, opr);
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
				QString nick_;
				QString city_;
				QString state_;
				QString country_;
				QString tablet_;
				std::list<ScoutingDataMapPtr> team_data_;
				QJsonObject ranking_;
				ScoutingDataMapPtr extra_data_;
			};
		}
	}
}
