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

#include "District.h"
#include "Event.h"

namespace xero
{
	namespace ba
	{


		District::District(const QString& key, const QString& abbrev, const QString& name, int year)
		{
			key_ = key;
			abbrev_ = abbrev;
			name_ = name;
			year_ = year;
		}

		District::~District()
		{
		}

		void District::addEvent(std::shared_ptr<Event> ev)
		{
			auto it = std::lower_bound(events_.begin(), events_.end(), ev,
				[](auto lhs, auto rhs) -> bool { return lhs->start() < rhs->start(); });

			events_.insert(it, ev);
		}

		void District::addTeam(std::shared_ptr<Team> team)
		{
			teams_.push_back(team);
		}

	}
}
