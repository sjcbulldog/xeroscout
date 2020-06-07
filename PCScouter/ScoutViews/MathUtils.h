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

