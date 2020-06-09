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

namespace xero 
{
	namespace paths
	{
		class MathUtils
		{
		public:
			MathUtils() = delete;
			~MathUtils() = delete;

			static double constexpr kEpsilon = 1e-5;
			static double constexpr kPI = 3.14159265358979323846264338327950288419716939937510582097494459230;

			static bool epsilonEqual(double a, double b, double ep = kEpsilon);
			static double interpolate(double first, double second, double percent);
			static double radiansToDegrees(double rads);
			static double degreesToRadians(double degrees);

			static double boundDegrees(double d) {
				if (d > 180.0)
				{
					while (d > 180.0)
						d -= 360;
				}
				else if (d <= -180)
				{
					while (d <= -180.0)
						d += 360;
				}

				return d;
			}

			static double BoundRadians(double r)
			{
				if (r > MathUtils::kPI)
					r -= 2 * MathUtils::kPI;
				else if (r <= -MathUtils::kPI)
					r += 2 * MathUtils::kPI;

				return r;
			}
		};
	}
}

