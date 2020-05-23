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

#include "EventWeekCalculator.h"
#include <QString>
#include <QDate>

namespace xero
{
	namespace ba
	{
		class District;
		class Match;

		class Event
		{
		public:
			Event(QString key, QString name, QString city, QString state, QString country, QString sdate, QString edate, int year)
			{
				key_ = key;
				name_ = name;
				city_ = city;
				state_ = state;
				country_ = country;

				sdate_ = QDate::fromString(sdate, "yyyy-MM-dd");
				edate_ = QDate::fromString(edate, "yyyy-MM-dd");
				week_ = EventWeekCalculator::week(sdate_);
			}

			virtual ~Event()
			{
			}

			void setDistrict(std::shared_ptr<District> d)
			{
				district_ = d;
			}

			std::shared_ptr<District> district()
			{
				return district_;
			}

			const QString& key() const {
				return key_;
			}

			const QString& name() const {
				return name_;
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

			const QDate& start() const {
				return sdate_;
			}

			const QDate& end() const {
				return edate_;
			}

			int year() const {
				return year_;
			}

			int week() const {
				return week_;
			}

			void addMatch(std::shared_ptr<Match> match) {
				matches_.push_back(match);
			}

			const std::list<std::shared_ptr<Match>>& matches() {
				return matches_;
			}

		private:
			QString key_;
			QString name_;
			QString city_;
			QString state_;
			QString country_;
			QDate sdate_;
			QDate edate_;
			int year_;
			int week_;

			std::list<std::shared_ptr<Match>> matches_;
			std::shared_ptr<District> district_;
		};

	}
}
