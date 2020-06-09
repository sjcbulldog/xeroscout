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
