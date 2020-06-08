//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "MatchAlliance.h"
#include <QJsonObject>
#include <memory>

namespace xero
{
	namespace ba
	{

		class Match
		{
		public:
			Match(const QString& key, const QString& comp_level, int set_number, int match_number, int etime, int ptime, int atime, std::shared_ptr<MatchAlliance> red, std::shared_ptr<MatchAlliance> blue)
			{
				key_ = key;
				comp_level_ = comp_level;
				set_number_ = set_number;
				match_number_ = match_number;
				etime_ = etime;
				ptime_ = ptime;
				atime_ = atime;
				red_ = red;
				blue_ = blue;
			}

			virtual ~Match() {
			}

			const QString& key() {
				return key_;
			}

			const QString compLevel() {
				return comp_level_;
			}

			const int setNumber() {
				return set_number_;
			}

			const int matchNumber() {
				return match_number_;
			}

			int eTime() {
				return etime_;
			}

			int pTime() {
				return ptime_;
			}

			int aTime() {
				return atime_;
			}

			std::shared_ptr<MatchAlliance> red() {
				return red_;
			}

			std::shared_ptr<MatchAlliance> blue() {
				return blue_;
			}

			const QJsonObject& scoreBreakdown() {
				return score_breakdown_;
			}

			void setScoreBreakdown(const QJsonObject& obj) {
				score_breakdown_ = obj;
			}

			const QJsonObject& zebraData() const {
				return zebra_data_;
			}

			void setZebraData(const QJsonObject& obj) {
				zebra_data_ = obj;
			}

			bool hasVideos() const {
				return videos_.size() > 0;
			}

			void addVideo(const QString& type, const QString& key) {
				videos_.push_back(std::make_pair(type, key));
			}

			const std::list<std::pair<QString, QString>>& videos() const {
				return videos_;
			}

		private:
			QString key_;
			QString comp_level_;
			int set_number_;
			int match_number_;
			QString winner_;
			int etime_;
			int ptime_;
			int atime_;
			std::shared_ptr<MatchAlliance> red_;
			std::shared_ptr<MatchAlliance> blue_;
			QJsonObject score_breakdown_;
			QJsonObject zebra_data_;
			std::list<std::pair<QString, QString>> videos_;
		};

	}
}
