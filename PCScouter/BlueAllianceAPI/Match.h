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
