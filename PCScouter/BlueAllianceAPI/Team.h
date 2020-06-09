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

#include <QString>
#include <QJsonObject>

namespace xero
{
	namespace ba
	{


		class Team
		{
		public:
			Team(QString key, int num, QString nick, QString name, QString city, QString state, QString country)
			{
				key_ = key;
				num_ = num;
				nick_ = nick;
				name_ = name;
				city_ = city;
				state_ = state;
				country_ = country;
			}

			virtual ~Team()
			{
			}

			void setRanking(const QJsonObject& obj) {
				ranking_ = obj;
			}

			const QJsonObject& ranking() {
				return ranking_;
			}

			const QString& key() const {
				return key_;
			}

			const int num() const {
				return num_;
			}

			const QString& nick() const {
				return nick_;
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

			void addEvent(const QString& evkey) {
				events_.push_back(evkey);
			}

			QStringList events() const {
				return events_;
			}

		private:
			QString key_;
			int num_;
			QString nick_;
			QString name_;
			QString city_;
			QString state_;
			QString country_;
			QStringList events_;
			QJsonObject ranking_;
		};
	}
}

