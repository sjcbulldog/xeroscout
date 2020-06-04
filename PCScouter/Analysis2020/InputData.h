#pragma once

//
// team, match, alliance, auto_line, auto_low, auto_high, tele_low, tele_high, wheel_spin, wheel_color, climbed, climb_assists, climb_was_assisted, park, balanced
// 2008,1,RED,1,66,53,49,41,0,1,1,0,0,0,0
//
// Calculated - balls_scored_auto
//            - balls_scored_tele
//            - balls_towards_shield
//

#include <string>
#include <vector>

namespace xero
{
	namespace analysis2020
	{
		typedef std::vector<double> row;

		class InputData
		{
		public:
			InputData();
			virtual ~InputData();

			bool readCSV(const std::string& file, std::string& error);

		private:
			std::vector<std::string> split(const std::string& line);

		private:
			std::vector<std::string> columns_;
			std::vector<row> data_;
		};
	}
}
