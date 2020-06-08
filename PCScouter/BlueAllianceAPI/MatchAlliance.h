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

#include <QStringList>

namespace xero
{
	namespace ba
	{
		class MatchAlliance
		{
		public:
			MatchAlliance() {
				score_ = -1;
			}

			virtual ~MatchAlliance() {
			}

			void setScore(int score) {
				score_ = score;
			}

			void addTeam(const QString& team) {
				teams_.push_back(team);
			}

			void addSurrogate(const QString& team) {
				surrogates_.push_back(team);
			}

			void addDQ(const QString& team) {
				dqs_.push_back(team);
			}

			QStringList& getTeams() {
				return teams_;
			}

			QStringList& getSurrogates() {
				return surrogates_;
			}

			QStringList& getDQs() {
				return dqs_;
			}

		private:
			QStringList teams_;
			QStringList surrogates_;
			QStringList dqs_;
			int score_;
		};

	}
}
