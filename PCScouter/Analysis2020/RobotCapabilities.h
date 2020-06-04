#pragma once

#include "Distribution.h"

namespace xero
{
	namespace analysis2020
	{
		class RobotCapabilities
		{
		public:
			RobotCapabilities();
			virtual ~RobotCapabilities();

		private:
			double auto_line_;
			double auto_low_;
			double auto_outer_;
			double auto_inner_;
			double tele_low_;
			double tele_outer_;
			double tele_inner_;
			Distribution auto_ball_dist_;
			Distribution tele_ball_dist_;
			double wheel_spin_;
			double wheel_color_;
			double climb_unassisted_;
		};
	}
}

