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

#include "EventWeekCalculator.h"
#include <time.h>
#include <chrono>

namespace xero
{
	namespace ba
	{


		std::vector<EventWeekCalculator::calcentry> EventWeekCalculator::weeks_ =
		{
			/* Year, start month, start day, end month, end day, week # */
			{ 2020, 2, 24, 3, 1, 1},
			{ 2020, 3, 2, 3, 8, 2},
			{ 2020, 3, 9, 3, 15, 3},
			{ 2020, 3, 16, 3, 22, 4},
			{ 2020, 3, 23, 3, 29, 5},
			{ 2020, 3, 30, 4, 5, 6},
			{ 2020, 4, 6, 4, 12, 7},
			{ 2020, 3, 13, 4, 19, 8},
		};


		int EventWeekCalculator::week(const QDate& d)
		{
			for (size_t i = 0; i < weeks_.size(); i++)
			{
				QDate startdate(weeks_[i].year, weeks_[i].smon, weeks_[i].sday);
				QDate enddate(weeks_[i].year, weeks_[i].emon, weeks_[i].eday);

				if (d >= startdate && d <= enddate)
					return weeks_[i].week;
			}

			return -1;
		}
	}
}
