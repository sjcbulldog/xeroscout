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
