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

