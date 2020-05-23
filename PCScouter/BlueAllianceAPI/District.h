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

#include "blueallianceapi_global.h"
#include <QString>
#include <list>

namespace xero
{
	namespace ba
	{


		class Team;
		class Event;

		class BLUEALLIANCEAPI_EXPORT District
		{
		public:
			District(const QString& key, const QString& abbrev, const QString& name, int year);
			virtual ~District();

			const QString& key() const { return key_; }
			const QString& abbrev() const { return abbrev_; }
			const QString& name() const { return name_; }
			int year() const { return year_; }

			void addEvent(std::shared_ptr<Event> ev);
			void addTeam(std::shared_ptr<Team> team);

			const std::list<std::shared_ptr<Event>>& events() const { return events_; }

		private:
			QString key_;
			QString abbrev_;
			QString name_;
			int year_;

			std::list<std::shared_ptr<Team>> teams_;
			std::list<std::shared_ptr<Event>> events_;
		};

	}
}
