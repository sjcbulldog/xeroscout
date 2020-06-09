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

#include "MathUtils.h"
#include <cmath>

namespace xero {
	namespace paths
	{
		bool MathUtils::epsilonEqual(double a, double b, double eps)
		{
			return std::fabs(a - b) < eps;
		}

		double MathUtils::interpolate(double a, double b, double percent)
		{
			if (percent < 0)
				percent = 0;
			else if (percent > 1)
				percent = 1;
			return a + (b - a) * percent;
		}

		double MathUtils::radiansToDegrees(double rads)
		{
			return rads / kPI * 180.0;
		}

		double MathUtils::degreesToRadians(double degrees)
		{
			return degrees / 180.0 * kPI;
		}


	}
}
