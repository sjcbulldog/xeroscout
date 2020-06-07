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
