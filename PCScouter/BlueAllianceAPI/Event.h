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

#include "EventWeekCalculator.h"
#include <QString>
#include <QDate>
#include <QDebug>

namespace xero
{
	namespace ba
	{
		class District;
		class Match;

		class Event
		{
		public:
			Event(QString key, QString name, QString city, QString state, QString country, QString sdate, QString edate)
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

			void addTeam(const QString& key) {
				team_keys_.push_back(key);
			}

			const QStringList& teamKeys() const {
				return team_keys_;
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

			QStringList team_keys_;
			std::list<std::shared_ptr<Match>> matches_;
			std::shared_ptr<District> district_;
		};

	}
}
