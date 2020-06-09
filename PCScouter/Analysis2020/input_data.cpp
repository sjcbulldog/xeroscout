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
#include "input_data.h"

Team rand(Team const*){
	//make a smaller set than is theoretically allowed to make it likely to have the same ones come up
	return Team{2000+rand()%75};
}

std::ostream& operator<<(std::ostream& o,Alliance a){
	switch(a){
		case Alliance::RED:
			return o<<"RED";
		case Alliance::BLUE:
			return o<<"BLUE";
		default:
			assert(0);
	}
}

Alliance parse(Alliance const*,std::string const& a){
	if(a=="RED" || a=="red") return Alliance::RED;
	if(a=="BLUE" || a=="blue") return Alliance::BLUE;
	throw "Invalid alliance: \""+a+"\"";
}

std::array<Alliance,2> options(Alliance const*){
	return {Alliance::RED,Alliance::BLUE};
}

Alliance rand(Alliance const*){
	return (rand()%2)?Alliance::RED:Alliance::BLUE;
}

std::vector<Alliance> alliances(){ return {Alliance::RED,Alliance::BLUE}; }

bool operator<(Input_row const& a,Input_row const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	INPUT_DATA(X)
	#undef X
	return 0;
}

#define SHOW(A,B) o<<""#B<<":"<<a.B<<" ";

std::ostream& operator<<(std::ostream& o,Input_row const& a){
	o<<"Input_row( ";
	INPUT_DATA(SHOW)
	return o<<")";
}

std::set<Team> teams(std::vector<Input_row> const& a){
	return to_set(mapf([](auto x){ return x.team; },a));
}

Input_row rand(Input_row const*){
	Input_row r;
	#define X(A,B) r.B=rand((A*)0);
	INPUT_DATA(X)
	#undef X
	//put some things in to make it more realistic
	if(r.climbed) r.park=0;
	if(r.climb_was_assisted || r.park) r.climb_assists=Climb_assists(0);
	if(!r.climbed){
	       r.climb_was_assisted=0;
	       r.climb_assists=Climb_assists(0);
	}
	return r;
}

std::vector<Input_row> read_csv(std::string const& path){
	std::ifstream f(path);

	std::string s;
	getline(f,s);
	auto columns=split(s,',');
	std::vector<std::string> expected_columns;
	#define X(A,B) expected_columns|=""#B;
	INPUT_DATA(X)
	#undef X
	if(columns!=expected_columns){
		std::cout<<"Column mismatch:\n";
		for(auto [i,p]:enumerate(zip_extend(expected_columns,columns))){
			if(p.first!=p.second){
				std::cout<<i<<"\t"<<p.first<<"\t"<<p.second<<"\n";
			}
		}
		exit(1);
	}

	std::vector<Input_row> r;
	while(f.good()){
		std::string s;
		getline(f,s);
		if(s=="") continue;
		auto data=split(s,',');
		if(data.size()!=columns.size()){
			PRINT(data.size());
			PRINT(columns.size());
		}
		assert(data.size()==columns.size());
		Input_row row;
		size_t i=0;
		#define X(A,B) row.B=parse((A*)0,data.at(i++));
		INPUT_DATA(X)
		#undef X
		r|=row;
	}
	return r;
}

//static const int TURN_THRESHOLD=29;
//static const int COLOR_PICK_THRESHOLD=49;

unsigned balls_scored_auto(Input_row const& a){
	//return a.auto_low+a.auto_outer+a.auto_inner;
	return a.auto_low+a.auto_high;
}

unsigned balls_scored_auto(std::vector<Input_row> const& a){
	return sum(MAP(balls_scored_auto,a));
}

unsigned balls_scored_tele(Input_row const& a){
	//return a.tele_low+a.tele_outer+a.tele_inner;
	return a.tele_low+a.tele_high;
}

unsigned balls_scored_tele(std::vector<Input_row> const& a){
	return sum(MAP(balls_scored_tele,a));
}

unsigned balls_scored(std::vector<Input_row> const& a){
	return balls_scored_auto(a)+balls_scored_tele(a);
}

unsigned balls_towards_shield(std::vector<Input_row> const& a){
	return min(balls_scored_auto(a),9)+balls_scored_tele(a);
}

void sanity_check(std::vector<Input_row> const& a){
	/*
	Sanity checks:
	-wheel turn when not enough balls
	-wheel colored when not enough balls
	-wheel colored when not turned
	-range of individual values
	-assists adding up on a team for climbs
	-not parking at the same time as climb
	-not having been assisted when parked
	-no giving assists if not climbed themselves
	-if more than one robot on an alliance credited with doing the same wheel item
	-the match numbers available look good

	also interesting to know:
	min/max/dist of each of the variables
	*/

	if(a.empty()){
		std::cout<<"No data found\n";
		exit(1);
	}

	std::cout<<"Seen data ranges:\n";
	#define X(A,B) {\
		auto values=mapf([](auto x){ return x.B; },a);\
		std::cout<<""#B<<":"<<min(values)<<" "<<max(values)<<" "<<take(50,as_string(dist(values)))<<"\n"; \
	}
	INPUT_DATA(X)
	#undef X
	std::cout<<"\n";

	auto matches=to_multiset(mapf([](auto x){ return x.match; },a));
	auto expected_range=range(1,max(matches)+1);
	for(auto i:expected_range){
		if(matches.count(Match(i))!=6){
			std::cout<<"Match "<<i<<": "<<matches.count(Match(i))<<" entries\n";
		}
	}

	//Check that the same robot does not appear more than once in the same match.
	for(auto [match,data]:group([](auto x){ return x.match; },a)){
		auto teams=to_multiset(mapf([](auto x){ return x.team; },data));
		for(auto team:to_set(teams)){
			if(teams.count(team)!=1){
				std::cout<<"Match "<<match<<" Team "<<team<<" appears more than once.\n";
			}
		}
	}

	auto by_alliance=group(
		[](auto x){ return std::make_pair(x.match,x.alliance); },
		a
	);

	for(auto [which,result]:sorted(to_vec(by_alliance))){
		auto marker=[=](){
			std::stringstream ss;
			ss<<"Match "<<which.first<<" Alliance "<<which.second<<": ";
			return ss.str();
		}();

		auto balls=balls_towards_shield(result);
		auto turns=sum(mapf([](auto x){ return x.wheel_spin; },result));
		auto color_picks=sum(mapf([](auto x){ return x.wheel_color; },result));

		if(turns && balls<TURN_THRESHOLD){
			std::cout<<marker<<"Too few balls scored ("<<balls<<") to score wheel turn.\n";
		}
		if(color_picks && balls<COLOR_PICK_THRESHOLD){
			std::cout<<marker<<"Too few balls scored ("<<balls<<") to score wheel color.\n";
		}
		if(turns>1){
			std::cout<<marker<<"More than one robot is given credit for scoring the wheel turn.\n";
		}

		auto assists_rx=sum(mapf([](auto x){ return x.climb_was_assisted; },result));
		auto assists_given=(unsigned)sum(mapf([](auto x){ return x.climb_assists; },result));
		if(assists_rx!=assists_given){
			std::cout<<marker<<"Climb assists given ("<<assists_given<<") is not equal to recieved ("<<assists_rx<<")\n";
		}
	}

	for(auto elem:sort_by(a,[](auto x){ return x.match; })){
		auto marker=[=](){
			std::stringstream ss;
			ss<<"Match "<<elem.match<<" Team "<<elem.team<<": ";
			return ss.str();
		}();

		if(elem.park && elem.climbed){
			std::cout<<marker<<"Parked but also marked as climbed.\n";
		}
		if(elem.park && elem.climb_assists){
			std::cout<<marker<<"Parked but also marked as assisting others in climb.\n";
		}
		if(elem.climb_assists && !elem.climbed){
			std::cout<<marker<<"Listed as assisted in climbs but did not climb itself.\n";
		}
		if(elem.climb_assists && elem.climb_was_assisted){
			std::cout<<marker<<"Listed as assisting in other climbs but own climb was assisted.\n";
		}
		if(!elem.climbed && elem.climb_was_assisted){
			std::cout<<marker<<"Listed as having been assisted in climb but did not climb.\n";
		}
	}
}

