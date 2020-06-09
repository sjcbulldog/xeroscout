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

#include "Rotation2d.h"
#include "Translation2d.h"
#include <cmath>
#include <limits>

namespace xero {
	namespace paths
	{

		Rotation2d::Rotation2d()
		{
			cos_angle_ = 1.0;
			sin_angle_ = 0.0;
		}

		Rotation2d::Rotation2d(double x, double y, bool normalize)
		{
			if (normalize)
			{
				double mag = std::hypot(x, y);
				if (mag > MathUtils::kEpsilon)
				{
					sin_angle_ = y / mag;
					cos_angle_ = x / mag;
				}
				else
				{
					sin_angle_ = 0.0;
					cos_angle_ = 1.0;
				}
			}
			else
			{
				cos_angle_ = x;
				sin_angle_ = y;
			}
		}

		Rotation2d::Rotation2d(const Rotation2d& other)
		{
			cos_angle_ = other.getCos();
			sin_angle_ = other.getSin();
		}

		Rotation2d::~Rotation2d()
		{
		}

		double Rotation2d::getTan() const
		{
			if (std::abs(cos_angle_) < MathUtils::kEpsilon) {
				if (sin_angle_ >= 0.0) {
					return std::numeric_limits<double>::infinity();
				}
				else {
					return -std::numeric_limits<double>::infinity();
				}
			}
			return sin_angle_ / cos_angle_;
		}

		double Rotation2d::toRadians() const
		{
			return std::atan2(sin_angle_, cos_angle_);
		}

		double Rotation2d::toDegrees() const
		{
			return MathUtils::radiansToDegrees(toRadians());
		}

		Rotation2d Rotation2d::fromRadians(double angle)
		{
			return Rotation2d(std::cos(angle), std::sin(angle), false);
		}

		Rotation2d Rotation2d::fromDegrees(double angle)
		{
			return fromRadians(MathUtils::degreesToRadians(angle));
		}

		Rotation2d Rotation2d::rotateBy(const Rotation2d& other) const
		{
			return Rotation2d(cos_angle_ * other.getCos() - sin_angle_ * other.getSin(),
				cos_angle_ * other.getSin() + sin_angle_ * other.getCos(), true);
		}

		Rotation2d Rotation2d::normal() const
		{
			return Rotation2d(-sin_angle_, cos_angle_, false);
		}

		Rotation2d Rotation2d::inverse() const
		{
			return Rotation2d(cos_angle_, -sin_angle_, false);
		}

		Rotation2d Rotation2d::interpolate(const Rotation2d& other, double percent) const
		{
			if (percent <= 0) {
				return *this;
			}
			else if (percent >= 1) {
				return other;
			}

			double angle_diff = inverse().rotateBy(other).toRadians();
			return rotateBy(Rotation2d::fromRadians(angle_diff * percent));
		}

		bool Rotation2d::epsilonEquals(const Rotation2d& other, double eps) const {
			return MathUtils::epsilonEqual(cos_angle_, other.getCos(), eps) &&
				MathUtils::epsilonEqual(sin_angle_, other.getSin(), eps);
		}

		bool Rotation2d::isParallel(const Rotation2d& other) const
		{
			Translation2d a(getCos(), getSin());
			Translation2d b(other.getCos(), other.getSin());

			return MathUtils::epsilonEqual(Translation2d::cross(a, b), 0.0);
		}
	}
}
