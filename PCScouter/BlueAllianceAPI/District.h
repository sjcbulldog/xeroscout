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

#include "blueallianceapi_global.h"
#include <QString>
#include <list>
#include <memory>

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
