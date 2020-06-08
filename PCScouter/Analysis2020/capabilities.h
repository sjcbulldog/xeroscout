//
// Copyright(C) 2020 by Eric Rippey
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
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#ifndef CAPABILITIES_H
#define CAPABILITIES_H

#include "dist.h"
#include "input_data.h"

#define CLIMB_CAPABILITIES(X)\
	X(Px,climb_unassisted)\
	X(Px,assist2)\
	X(Px,assist1)\
	X(Px,climb_was_assisted)\
	X(Px,park)\
	X(Px,balance)

#define ROBOT_CAPABILITIES(X)\
	X(Px,auto_line)\
	X(double,auto_low)\
	X(double,auto_outer)\
	X(double,auto_inner)\
	X(double,tele_low)\
	X(double,tele_outer)\
	X(double,tele_inner)\
	X(Dist,auto_ball_dist)\
	X(Dist,tele_ball_dist)\
	X(Px,wheel_spin)\
	X(Px,wheel_color)\
	CLIMB_CAPABILITIES(X)

struct Robot_capabilities{
	ROBOT_CAPABILITIES(INST)
};

/*
climb strategy:
1) all on own
2) each one attempting to assist each of the others
3) just park
map<Team,park/climb/assisted_climb>
*/

/*
 * Probability that can get to the bonus objectives
 * */

std::pair<Px,Px> bonus_balls_hit(std::array<Dist,3> const& auto_dist,std::array<Dist,3> const& tele_dist);

std::ostream& operator<<(std::ostream&,Robot_capabilities const&);

Robot_capabilities mean(std::vector<Robot_capabilities> const&);

bool operator<(Robot_capabilities const&,Robot_capabilities const&);

Px wheel_odds(Team team,std::vector<std::vector<Input_row>> const& alliance_results);
Px wheel_color(Team team,std::vector<std::vector<Input_row>> const& alliance_results);
double assisted2(Team team,std::vector<std::vector<Input_row>> const& alliance_results);
double assisted1(Team team,std::vector<std::vector<Input_row>> const& alliance_results);
Px park(std::vector<Input_row> const&);
Px balance(std::vector<Input_row> const& matches);
bool attempted_endgame(Input_row const&);

//static const int TELEOP_LENGTH=135;
//static const int ENDGAME_LENGTH=20;//going to assume that people only spend 20 of the 30 seconds trying to climb.
int teleop_length();//seconds
int endgame_length();//seconds

std::map<Team,Robot_capabilities> robot_capabilities(std::vector<Input_row> const& in);

#endif
