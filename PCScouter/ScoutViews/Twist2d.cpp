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

#include "Twist2d.h"
#include "MathUtils.h"
#include <cmath>

namespace xero {
	namespace paths
	{
		Twist2d Twist2d::kIdentity(0.0, 0.0, 0.0);

		Twist2d::Twist2d()
		{
			dx_ = 0.0;
			dy_ = 0.0;
			dtheta_ = 0.0;
		}

		Twist2d::Twist2d(double dx, double dy, double dtheta)
		{
			dx_ = dx;
			dy_ = dy;
			dtheta_ = dtheta;
		}

		Twist2d::~Twist2d()
		{
		}

		Twist2d Twist2d::scaled(double scale) const
		{
			return Twist2d(dx_ * scale, dy_ * scale, dtheta_ * scale);
		}

		double Twist2d::normal() const
		{
			if (dy_ == 0.0)
				return std::abs(dx_);

			return std::hypot(dx_, dy_);
		}

		double Twist2d::curvature() const
		{
			double dn = normal();

			if (std::abs(dtheta_) < MathUtils::kEpsilon && dn < MathUtils::kEpsilon)
				return 0.0;

			return dtheta_ / dn;
		}
	}
}
