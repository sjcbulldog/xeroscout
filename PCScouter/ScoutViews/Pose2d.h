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
#include "Translation2d.h"
#include "Rotation2d.h"
#include "Twist2d.h"
#include <string>

namespace xero
{
	namespace paths
	{
		class Pose2d 
		{
		public:
			Pose2d();
			Pose2d(double x, double y);
			Pose2d(double x, double y, const Rotation2d &rot);
			Pose2d(const Translation2d& pos);
			Pose2d(const Rotation2d& pos);
			Pose2d(const Translation2d& pos, const Rotation2d& rot);
			Pose2d(const Pose2d& other);
			virtual ~Pose2d();
			
			static Twist2d logfn(const Pose2d& transform);
			static Pose2d expfn(const Twist2d& delta);

			const Translation2d& getTranslation() const {
				return pos_;
			}

			const Rotation2d& getRotation() const {
				return rotation_;
			}

			Pose2d transformBy(const Pose2d& other) const;
			Pose2d inverse() const;
			Pose2d normal() const;
			Translation2d intersection(const Pose2d& other) const;
			bool isColinear(const Pose2d& other);
			bool espilonEquals(const Pose2d& other, double eps = MathUtils::kEpsilon) const;
			bool espilonEquals(const Pose2d& other, double poseps = MathUtils::kEpsilon, double roteps = MathUtils::kEpsilon) const;
			Pose2d interpolate(const Pose2d& other, double percent) const;
			double distance(const Pose2d &other) const;
			Pose2d mirror() const;

			virtual double getField(const std::string& field) const;

		private:
			static Translation2d intersection(const Pose2d& a, const Pose2d& b);

		private:
			Translation2d pos_;
			Rotation2d rotation_;
		};
	}
}
