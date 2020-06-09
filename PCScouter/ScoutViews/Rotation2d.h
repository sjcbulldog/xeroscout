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
