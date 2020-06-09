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

#include "BlueAllianceResult.h"

namespace xero
{
	namespace ba
	{

		class BACountResult : public BlueAllianceResult
		{
		public:
			BACountResult(int count, int total, bool known) : BlueAllianceResult(BlueAllianceResult::Status::Success)
			{
				count_ = count;
				total_ = total;
				total_known_ = known;
			}

			virtual ~BACountResult()
			{
			}

			int count() const { return count_; }
			int total() const { return total_; }
			bool isTotalKnown() const { return total_known_; }
			bool isDone() const { return total_known_ && count_ == total_; }

		private:
			int count_;
			int total_;
			bool total_known_;
		};

	}
}
