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
#include "Rotation2d.h"

namespace xero {
	namespace paths
	{
		class Rotation2d;

		class Translation2d
		{
		public:
			Translation2d();
			Translation2d(double x, double y);
			Translation2d(const Translation2d& other);
			Translation2d(const Translation2d& start, const Translation2d& end);
			Translation2d(const Rotation2d& rot, double magnitude);
			~Translation2d();

			static Translation2d kIdentity;
			static double dot(const Translation2d& a, const Translation2d& b);
			static double cross(const Translation2d& a, const Translation2d& b);
			static Rotation2d getAngle(const Translation2d& a, const Translation2d& b);

			double getX() const { return x_; }
			double getY() const { return y_; }
			Rotation2d toRotation() const;

			double normalize() const;
			double normalizeSquared() const ;

			Translation2d translateBy(const Translation2d& translation) const ;
			Translation2d rotateBy(const Rotation2d& rotation) const ;
			Translation2d inverse() const;
			Translation2d interpolate(const Translation2d& other, double percent) const;
			Translation2d extrapolate(const Translation2d& other, double percent) const;
			Translation2d scale(double scale) const;
			Rotation2d direction() const;
			double distance(const Translation2d& other) const;
			bool epsilonEqual(const Translation2d& other, double eps = MathUtils::kEpsilon) const;

		private:
			double x_;
			double y_;
		};

		static inline Translation2d operator+(const Translation2d& a, const Translation2d& b)
		{
			return Translation2d(a.getX() + b.getX(), a.getY() + b.getY());
		}
	}
}

