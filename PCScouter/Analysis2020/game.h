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
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#ifndef GAME_H
#define GAME_H

#include "capabilities.h"

Alliance operator-(Alliance);

using Alliance_station_number=Int_limited<1,3>;

#define ALLIANCE_STATION_ITEMS(X)\
	X(Alliance,alliance)\
	X(Alliance_station_number,number)

struct Alliance_station{
	ALLIANCE_STATION_ITEMS(INST)
};

bool operator==(Alliance_station const&,Alliance_station const&);
bool operator<(Alliance_station const&,Alliance_station const&);
std::ostream& operator<<(std::ostream&,Alliance_station const&);

std::vector<Alliance_station> options(Alliance_station const*);

using Alliance_capabilities=std::array<Robot_capabilities,3>;

enum class Climb_strategy{
	NONE,
	PARK,
	CLIMB_SELF,
	CLIMB_ASSISTED
};

std::ostream& operator<<(std::ostream&,Climb_strategy const&);
std::vector<Climb_strategy> climb_strategies();

using Alliance_climb_strategy=std::array<Climb_strategy,3>;

std::vector<Alliance_climb_strategy> alliance_climb_strategies();

struct Climb_capabilities{
	CLIMB_CAPABILITIES(INST)
};

std::ostream& operator<<(std::ostream&,Climb_capabilities const&);

using Alliance_climb_capabilities=std::array<Climb_capabilities,3>;

double climb_points(Alliance_climb_capabilities const&,Alliance_climb_strategy const&);

double climb_points(Alliance_climb_capabilities const&);

using Alliance_strategy=Alliance_climb_strategy;

double expected_score(Alliance_capabilities const&,Alliance_strategy const&);
double expected_score(Alliance_capabilities const&);

void show(std::map<Team,Robot_capabilities> const& data);

using Picklist=std::vector<
	std::pair<
		std::pair<double,Team>,
		std::vector<std::pair<double,Team>>
	>
>;

void show_picklist(Team picker,Picklist const&);

Picklist make_picklist(Team picking_team,std::map<Team,Robot_capabilities>);

#endif
