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
				DataModelTeam(const QString& key, int number, const QString& name) {
					key_ = key;
					number_ = number;
					name_ = name;
				}

				virtual ~DataModelTeam() {
				}

				bool hasRank() const {
					return ranking_.contains("rank") && ranking_.value("rank").isDouble();
				}

				int rank() const {
					return ranking_.value("rank").toInt();
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

				ConstScoutingDataMapPtr pitScoutingData() const {
					if (pit_data_.size() == 0)
						return nullptr;

					return pit_data_.back();
				}

				std::list<ConstScoutingDataMapPtr> pitScoutingDataList() const {
					std::list<ConstScoutingDataMapPtr> ret;

					for (auto p : pit_data_)
						ret.push_back(p);

					return ret;
				}

				const QString& tablet() const {
					return tablet_;
				}

				void removeOldScoutingData() {
					while (pit_data_.size() > 1)
						pit_data_.erase(pit_data_.begin());
				}

			protected:
				void setRanking(const QJsonObject& obj) {
					ranking_ = obj;
				}

				void setNumber(int n) {
					number_ = n;
				}

				void setName(const QString& name) {
					name_ = name;
				}

				void clearPitScoutingData() {
					pit_data_.clear();
				}

				bool setPitScoutingData(ScoutingDataMapPtr d, bool replace) {
					assert(d != nullptr);

					if (replace)
					{
						pit_data_.clear();
					}
					else
					{
						for (auto pd : pit_data_)
						{
							if (compareTwoMaps(pd, d))
								return false;
						}
					}

					pit_data_.push_back(d);
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
				std::list<ScoutingDataMapPtr> pit_data_;
				QJsonObject ranking_;
			};
		}
	}
}
