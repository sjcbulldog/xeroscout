//
// Copyright 2020 by Eric Rippey
//
// Courtesy of FRC Team Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#include "game.h"

Alliance operator-(Alliance a){
	switch(a){
		case Alliance::RED:
			return Alliance::BLUE;
		case Alliance::BLUE:
			return Alliance::RED;
		default:
			assert(0);
	}
}

bool operator==(Alliance_station const& a,Alliance_station const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	ALLIANCE_STATION_ITEMS(X)
	#undef X
	return 1;
}

bool operator<(Alliance_station const& a,Alliance_station const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	ALLIANCE_STATION_ITEMS(X)
	#undef X
	return 0;
}

std::ostream& operator<<(std::ostream& o,Alliance_station const& a){
	o<<"(";
	ALLIANCE_STATION_ITEMS(SHOW)
	return o<<")";
}

std::vector<Alliance_station> options(Alliance_station const*){
	std::vector<Alliance_station> r;
	for(auto a:options((Alliance*)0)){
		for(auto b:options((Alliance_station_number*)0)){
			r|=Alliance_station{a,b};
		}
	}
	return r;
}

std::vector<Climb_strategy> climb_strategies(){
	return std::vector<Climb_strategy>{
		Climb_strategy::NONE,
		Climb_strategy::PARK,
		Climb_strategy::CLIMB_SELF,
		Climb_strategy::CLIMB_ASSISTED
	};
}

std::ostream& operator<<(std::ostream& o,Climb_strategy const& a){
	#define X(A) if(a==Climb_strategy::A) return o<<""#A;
	X(NONE) X(PARK) X(CLIMB_SELF) X(CLIMB_ASSISTED)
	#undef X
	assert(0);
}

using Alliance_climb_strategy=std::array<Climb_strategy,3>;

std::vector<Alliance_climb_strategy> alliance_climb_strategies(){
	std::vector<Alliance_climb_strategy> r;
	for(auto a:climb_strategies()){
		for(auto b:climb_strategies()){
			for(auto c:climb_strategies()){
				r|=Alliance_climb_strategy{a,b,c};
			}
		}
	}
	return r;
}

std::ostream& operator<<(std::ostream& o,Climb_capabilities const& a){
	o<<"Climb_capabilities( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	CLIMB_CAPABILITIES(X)
	#undef X
	return o<<")";
}

double climb_points(Alliance_climb_capabilities const& cap,Alliance_climb_strategy const& strat){
	auto z=zip(cap,strat);
	double park_points=0;
	std::vector<double> assists_available;
	std::vector<double> assists_req;
	double climbed=0;
	std::vector<Px> balancers;
	for(auto [cap1,strat1]:z){
		switch(strat1){
			case Climb_strategy::NONE:
				break;
			case Climb_strategy::PARK:
				park_points+=5*cap1.park;
				break;
			case Climb_strategy::CLIMB_SELF:
				climbed+=cap1.climb_unassisted;
				balancers|=cap1.balance;
				break;
			case Climb_strategy::CLIMB_ASSISTED:
				assists_req|=cap1.climb_was_assisted;
				break;
			default:
				assert(0);
		}
	}
	for(auto [a,b]:zip(
		reversed(sorted(assists_available)),
		reversed(sorted(assists_req))
	)){
		climbed+=mean(a,b);//geometric mean might be better
	}
	auto balance_points=balancers.empty()?0:(15*geomean(balancers));
	return park_points+climbed*25+balance_points;
}

double climb_points(Alliance_climb_capabilities const& cap){
	return max(mapf(
		[=](auto x){ return climb_points(cap,x); },
		alliance_climb_strategies()
	));
}

using Alliance_strategy=Alliance_climb_strategy;

double expected_score(Alliance_capabilities const& a,Alliance_strategy const& strat){
	//at some point might want to make this fancier and do things like scoring rates based on the amount of time
	//that might be used for the other activities
	//also, it would be good if had some idea of what the distribution of # of balls was so that could estimate
	//how likely it would be to get alliances to the different thresholds rather than just having a binary
	//yes/no on them.
	auto auto_line_points=5*sum(mapf([](auto x){ return x.auto_line; },a));

	auto auto_low=sum(mapf([](auto x){ return x.auto_low; },a));
	auto auto_outer=sum(mapf([](auto x){ return x.auto_outer; },a));
	auto auto_inner=sum(mapf([](auto x){ return x.auto_inner; },a));

	auto sum_scaled=[=](std::array<double,3> a)->double{
		double r=0;
		for(auto [value,climb]:zip(a,strat)){
			if(climb==Climb_strategy::NONE){
				r+=value;
			}else{
				static const double SCALE_FACTOR=(teleop_length()-endgame_length()+0.0)/teleop_length();
				r+=value*SCALE_FACTOR;
			}
		}
		return r;
	};
	auto tele_low=sum_scaled(mapf([](auto x){ return x.tele_low; },a));
	auto tele_outer=sum_scaled(mapf([](auto x){ return x.tele_outer; },a));
	auto tele_inner=sum_scaled(mapf([](auto x){ return x.tele_inner; },a));

	auto ball_points=2*auto_low+4*auto_outer+6*auto_inner+tele_low+tele_outer*2+tele_inner*3;
	//auto auto_balls=auto_low+auto_outer+auto_inner;
	//auto tele_balls=tele_low+tele_outer+tele_inner;
	//auto balls_towards_shield=min(auto_balls,9)+tele_balls;

	//Could worry about whether or not the teleop # of balls is correlated with the auto number of balls
	//for example, if don't get certain shots off in auto then have the balls to shoot sooner in tele

	auto [p_spin_available,p_color_available]=bonus_balls_hit(
		mapf([](auto x){ return x.auto_ball_dist; },a),
		mapf([](auto x){ return x.tele_ball_dist; },a)
	);

	auto spin_points=[&]()->double{
		auto p=max(mapf([](auto x){ return x.wheel_spin; },a));
		return 10*p*p_spin_available;
	}();

	auto color_pick_points=[&]()->double{
		auto p=max(mapf([](auto x){ return x.wheel_color; },a));
		return 20*p*p_color_available;
	}();

	//hang points
	//hang strategies:
	//does each one want to hang?
	//for each of them, assume that they have y/n for attempt to do something for each team
	//then for each of those categories, choose between park/climb/get assisted (if available)
	//
	auto hang_points=climb_points(
		mapf(
			[](auto x){
				Climb_capabilities r;
				#define X(A,B) r.B=x.B;
				CLIMB_CAPABILITIES(X)
				#undef X
				return r;
			},
			a
		),
		strat
	);

	return auto_line_points+ball_points+spin_points+color_pick_points+hang_points;
}

double expected_score(Alliance_capabilities const& a){
	return max(mapf(
		[=](auto x){ return expected_score(a,x); },
		alliance_climb_strategies()
	));
}

void show(std::map<Team,Robot_capabilities> const& data){
	auto heading="Robot capabilities";
	std::string s=html(
		head(
			title(heading)
		)
		+body(
			h1(heading)
			+tag("table border",
				tr(
					th("Team")
					#define X(A,B) +th(""#B)
					ROBOT_CAPABILITIES(X)
					#undef X
				)+
				join(mapf(
					[](auto x){
						auto [team,data]=x;
						return tr(
							td(team)
							#define X(A,B) +td(data.B)
							ROBOT_CAPABILITIES(X)
							#undef X
						);
					},
					data
				))
			)
		)
	);
	write_file("robot_capabilities.html",s);
}

using Picklist=std::vector<
	std::pair<
		std::pair<double,Team>,
		std::vector<std::pair<double,Team>>
	>
>;

void show_picklist(Team picker,Picklist const& a){
	auto heading="Team "+as_string(picker)+" Picklist";

	auto show_box=[](std::pair<double,Team> p)->std::string{
		return td(
			as_string(p.second)
			+"<br>"
			+tag(
				"small",
				tag("font color=grey",p.first)
			)
		);
	};

	auto s=html(
		head(title(heading))
		+body(
			h1(heading)+
			tag("table border",
				tr(
					tag("th colspan=2","First pick")+
					tag("th colspan=22","Second pick")
				)+
				tr(
					th("Rank")+
					th("Team")+
					join(mapf([](auto i){ return th(i); },range(1,23)))
				)+
				join(mapf(
					[=](auto p){
						auto [i,x]=p;
						auto [fp,second]=x;
						return tr(
							th(i)
							+show_box(fp)
							+join(mapf(show_box,take(22,second)))
						);
					},
					enumerate_from(1,take(15,a))
				))
			)
		)
	);
	write_file("picklist.html",s);
}

Picklist make_picklist(Team picking_team,std::map<Team,Robot_capabilities> a){
	auto picking_cap=a[picking_team];
	auto others=filter_keys(
		[=](auto k){
			return k!=picking_team;
		},
		a
	);
	
	auto x=reversed(sorted(mapf(
		[=](auto p){
			auto [t1,t1_cap]=p;
			(void)t1;
			Alliance_capabilities cap{picking_cap,t1_cap,{}};
			return std::make_pair(expected_score(cap),t1_cap);
		},
		others
	)));

	auto ex_cap=[&]()->Robot_capabilities{
		if(others.size()==0){
			return {};
		}
		if(others.size()>24){
			//take the 22nd-24th best robots as the example 3rd robot
			return mean(take(5,skip(22,seconds(x))));
		}
		//if fewer than 24, then just take the bottom 5.
		return mean(take(5,reversed(seconds(x))));
	}();

	std::cout<<"Example 3rd robot capabilities:\n";
	std::cout<<ex_cap<<"\n";

	auto first_picks=reversed(sorted(mapf(
		[=](auto p){
			auto [t1,t1_cap]=p;
			Alliance_capabilities cap{picking_cap,t1_cap,ex_cap};
			return std::make_pair(expected_score(cap),t1);
		},
		others
	)));

	//PRINT(first_picks);

	auto second_picks=to_map(mapf(
		[&](auto t1)->std::pair<Team,std::vector<std::pair<double,Team>>>{
			auto t1_cap=a[t1];
			auto left=without_key(t1,others);
			auto result=reversed(sorted(mapf(
				[=](auto x){
					auto [t2,t2_cap]=x;
					Alliance_capabilities cap{picking_cap,t1_cap,t2_cap};
					return std::make_pair(expected_score(cap),t2);
				},
				to_vec(left)
			)));
			return std::make_pair(t1,result);
		},
		to_vec(keys(others))
	));

	return mapf(
		[&](auto p){
			return std::make_pair(p,second_picks[p.second]);
		},
		first_picks
	);
}

