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

namespace xero
{
	namespace ba
	{


		class BlueAllianceResult
		{
		public:
			enum class Status
			{
				Success,
				SiteDown,
				ConnectionError,
				SSLError,
				JSONError
			};

		public:
			BlueAllianceResult(Status st)
			{
				st_ = st;
			}

			virtual ~BlueAllianceResult()
			{
			}

			Status status()
			{
				return st_;
			}

			QString toString()
			{
				return toString(st_);
			}

			static QString toString(Status st)
			{
				QString result;

				switch (st)
				{
				case Status::Success:
					result = "success";
					break;
				case Status::ConnectionError:
					result = "connection error";
					break;
				case Status::JSONError:
					result = "json error";
					break;
				case Status::SSLError:
					result = "ssl error";
					break;
				}

				return result;
			}

		private:
			Status st_;
		};
	}
}
