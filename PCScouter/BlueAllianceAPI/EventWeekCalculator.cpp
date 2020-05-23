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
