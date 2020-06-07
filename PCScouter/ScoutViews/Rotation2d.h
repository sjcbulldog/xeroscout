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

#include "MathUtils.h"

namespace xero 
{
	namespace paths
	{
		class Rotation2d
		{
		public:
			Rotation2d();
			Rotation2d(double x, double y, bool normalize);
			Rotation2d(const Rotation2d& other);
			~Rotation2d();

			double getCos() const { return cos_angle_; }
			double getSin() const { return sin_angle_; }
			double getTan() const;
			double toRadians() const;
			double toDegrees() const;

			static Rotation2d fromRadians(double angle);
			static Rotation2d fromDegrees(double angle);

			Rotation2d rotateBy(const Rotation2d& other) const;
			Rotation2d normal() const;
			Rotation2d inverse() const;
			Rotation2d interpolate(const Rotation2d& other, double percent) const;

			bool isParallel(const Rotation2d& other) const;
			bool epsilonEquals(const Rotation2d& other, double eps = MathUtils::kEpsilon) const;


		private:
			double sin_angle_;
			double cos_angle_;
		};
	}
}
