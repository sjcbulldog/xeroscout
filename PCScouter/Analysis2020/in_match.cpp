#include<iostream>
#include<functional>
#include<cmath>
#include "util.h"
#include "input_data.h"
#include "game.h"

using namespace std;

vector<int> range(int min,int lim,int step){
	vector<int> r;
	for(int i=min;i<lim;i+=step){
		r|=i;
	}
	return r;
}

template<typename T>
T choose(std::vector<T> const& a){
	return a[rand()%a.size()];
}

template<typename T,size_t N>
std::array<std::pair<size_t,T>,N> enumerate(std::array<T,N> const& a){
	std::array<std::pair<size_t,T>,N> r;
	for(auto i:range_st<N>()){
		r[i]=make_pair(i,a[i]);
	}
	return r;
}

template<typename T,size_t N,size_t M>
std::array<T,N*M> flatten(std::array<std::array<T,N>,M> const&){
	nyi
}

template<typename T,size_t N>
std::vector<T> flatten(std::vector<std::array<T,N>> const& a){
	std::vector<T> r;
	for(auto const& elem:a){
		r|=elem;
	}
	return r;
}

template<typename A,typename B>
std::vector<std::pair<A,B>> options(std::pair<A,B> const*){
	return cross(options((A*)0),options((B*)0));
}

template<typename A,size_t N,typename B>
std::vector<std::pair<A,B>> cross(std::array<A,N> const& a,std::vector<B> const& b){
	std::vector<std::pair<A,B>> r;
	for(auto const& a1:a){
		for(auto const& b1:b){
			r|=make_pair(a1,b1);
		}
	}
	return r;
}

template<typename Func,typename K,typename V>
auto map_map(Func f,std::map<K,V> const& a){
	std::map<K,decltype(f(*begin(a)))> r;
	for(auto p:a){
		r[p.first]=f(p);
	}
	return r;
}

/*template<typename T>
vector<T> operator+(vector<T> a,vector<T> b){
	a|=b;
	return a;
}*/

template<typename Func,typename T>
vector<T> sort_by(Func f,vector<T> a){
	std::sort(
		begin(a),
		end(a),
		[=](auto e1,auto e2)->bool{
			return f(e1)<f(e2);
		}
	);
	return a;
}

using Score=unsigned;
using Scores=map<Alliance,unsigned>;
using Balls_by_alliance=map<Alliance,unsigned>;
using On_robot_balls=map<Alliance_station,unsigned>;//should be 0-5.

using Ball_score_location=Int_limited<1,3>;
using Balls_scored=map<Ball_score_location,unsigned>;
using Time=int;

#define CYCLE_ITEMS(X)\
	X(Time,time)\
	X(Balls_scored,scored)

struct Cycle{
	CYCLE_ITEMS(INST)
};

std::ostream& operator<<(std::ostream& o,Cycle const& a){
	o<<"Cycle(";
	CYCLE_ITEMS(SHOW)
	return o<<")";
}

//locations initially are just which end of the field
//	X(bool,at_shooting_end)
//
using Location=int;

bool at_shooting_end(Location a){
	return a>=2;
}

bool on_near_end(Location a){
	return a<=0;
}

#define ROBOT_STATE_ITEMS(X)\
	X(Location,location)\
	X(unsigned,balls_held)

struct Robot_state{
	ROBOT_STATE_ITEMS(INST)
};

std::ostream& operator<<(std::ostream& o,Robot_state const& a){
	o<<"Robot_state(";
	ROBOT_STATE_ITEMS(SHOW)
	return o<<")";
}

static const int MAX_BALLS_HELD=5;

#define ROBOT_ACTIONS(X)\
	X(PICKUP)\
	X(LOAD_STATION)\
	X(SHOOT1)\
	X(SHOOT2)\
	X(SHOOT3)\
	X(DRIVE_TO_OWN_END)\
	X(DRIVE_TO_SHOOTING_END)\
	X(WAIT)

enum class Robot_action{
	#define X(A) A,
	ROBOT_ACTIONS(X)
	#undef X
};

std::ostream& operator<<(std::ostream& o,Robot_action a){
	#define X(A) if(a==Robot_action::A) return o<<""#A;
	ROBOT_ACTIONS(X)
	#undef X
	assert(0);
}

vector<Robot_action> options(Robot_action const*){
	return {
		#define X(A) Robot_action::A,
		ROBOT_ACTIONS(X)
		#undef X
	};
}

Robot_action rand(Robot_action const *a){
	return choose(options(a));
}

Robot_action choose_action(Robot_state state,bool balls_for_pickup_available_shooting,bool balls_for_pickup_available_far){
	if(at_shooting_end(state.location)){
		if(state.balls_held<MAX_BALLS_HELD && balls_for_pickup_available_shooting){
			return Robot_action::PICKUP;
		}
		if(state.balls_held){
			return Robot_action::SHOOT3;
		}
		return Robot_action::DRIVE_TO_OWN_END;
	}
	if(on_near_end(state.location)){
		if(state.balls_held==MAX_BALLS_HELD){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		if(balls_for_pickup_available_far){
			return Robot_action::PICKUP;
		}
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	if(state.balls_held){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	if(balls_for_pickup_available_shooting){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return Robot_action::DRIVE_TO_OWN_END;
}

static const int ROBOTS_PER_MATCH=6;
static const int ROBOTS_PER_ALLIANCE=3;

using Robot_strategy=std::function<Robot_action(Robot_state,int,int)>;

using Strategy=std::map<Alliance_station,Robot_strategy>;
//using Alliance_robots=std::array<Robot_state,ROBOTS_PER_ALLIANCE>;
//using Robots=std::array<Robot_state,ROBOTS_PER_MATCH>;
using Robots=map<Alliance_station,Robot_state>;

#define GAME_STATE_ITEMS(X)\
	X(Scores,scores)\
	X(Balls_by_alliance,driver_station_balls)\
	X(Balls_by_alliance,floor_balls)\
	X(Robots,robots)
	
struct Game_state{
	GAME_STATE_ITEMS(INST)

	Game_state(){
		for(auto i:options((Alliance_station*)0)){
			robots[i].balls_held=3;
		}
		for(auto i:options((Alliance*)0)){
			floor_balls[i]=2*5;//trench and center
			driver_station_balls[i]=5;;
		}
	}
};

std::ostream& operator<<(std::ostream& o,Game_state const& a){
	o<<"Game_state( ";
	GAME_STATE_ITEMS(SHOW)
	return o<<")";
}

pair<map<Alliance_station,Robot_action>,Game_state> next(Game_state in,Strategy strategy){
	auto chosen=map_map(
		[&](auto p){
			auto [k,x]=p;
			auto balls_at_shooting=in.floor_balls[-k.alliance];
			auto balls_at_home=in.floor_balls[k.alliance]+in.driver_station_balls[k.alliance];
			/*if(k==Alliance_station{Alliance::RED,Alliance_station_number{1}}){
				return rand((Robot_action*)0);
			}
			return choose_action(x,balls_at_shooting,balls_at_home);*/
			return strategy[k](x,balls_at_shooting,balls_at_home);
		},
		in.robots
	);
	//PRINT(values(chosen));
	for(auto [location,action]:chosen){
		auto& robot=in.robots[location];
		auto current_end=[=]()->optional<Alliance>{
			if(at_shooting_end(robot.location)){
				return -location.alliance;
			}
			if(on_near_end(robot.location)){
				return location.alliance;
			}
			return {};
		}();

		auto shoot=[&](int pts){
			if(!at_shooting_end(robot.location)) return;

			//don't have to check this since it's just a nop is don't have any balls
			//assert(robot.balls_held);

			auto to_shoot=robot.balls_held;
			//auto to_shoot=min(1,robot.balls_held);
			in.scores[location.alliance]+=pts*to_shoot;
			in.driver_station_balls[-location.alliance]+=to_shoot;
			robot.balls_held-=to_shoot;
		};

		switch(action){
			case Robot_action::PICKUP:
				if(robot.balls_held>=MAX_BALLS_HELD) break;
				if(current_end!=std::nullopt){
					if(in.floor_balls[*current_end]){
						//now, always assuming there are balls
						robot.balls_held++;
						//PRINT(in.floor_balls[current_end]);
						in.floor_balls[*current_end]--;
						//cout<<"after "<<in.floor_balls[current_end]<<"\n";
					}else if(in.driver_station_balls[location.alliance] && on_near_end(robot.location)){
						robot.balls_held++;
						in.driver_station_balls[location.alliance]--;
					}else{
						//just do nothing if ball not there
					}
				}else{
					//if in transit, do not allow pickup
				}
				break;
			case Robot_action::LOAD_STATION:
				if(robot.balls_held>=MAX_BALLS_HELD) break;
				if(!on_near_end(robot.location)) break;
				if(in.driver_station_balls[location.alliance]==0) break;
				robot.balls_held++;
				in.driver_station_balls[location.alliance]--;
				break;
			case Robot_action::DRIVE_TO_SHOOTING_END:
				//assert(!robot.at_shooting_end);
				//robot.at_shooting_end=1;
				robot.location++;
				break;
			case Robot_action::SHOOT1:
				shoot(1);
				break;
			case Robot_action::SHOOT2:
				shoot(2);
				break;
			case Robot_action::SHOOT3:
				shoot(3);
				break;
			case Robot_action::DRIVE_TO_OWN_END:
				//assert(robot.at_shooting_end);
				//robot.at_shooting_end=0;
				robot.location--;
				break;
			case Robot_action::WAIT:
				break;
			default:
				PRINT(action);
				PRINT(robot);
				nyi
		}
	}
	for(auto i:alliances()){
		static const int MAX_DRIVER_STATION_BALLS=15;
		while(in.driver_station_balls[i]>MAX_DRIVER_STATION_BALLS){
			in.driver_station_balls[i]--;
			in.floor_balls[i]++;
			//dropped onto floor
		}
	}
	return make_pair(chosen,in);
}

bool valid(Game_state const& a){
	auto balls=sum(values(a.driver_station_balls)+values(a.floor_balls)+mapf([](auto x){ return x.balls_held; },values(a.robots)));
	return balls==48;
}

vector<Robot_action> legal_actions(Robot_state state,bool ball_shooting_end,bool ball_own_end){
	(void)ball_shooting_end;
	(void)ball_own_end;
	vector<Robot_action> r;
	r|=Robot_action::WAIT;
	if(at_shooting_end(state.location)){
		if(ball_shooting_end && state.balls_held<MAX_BALLS_HELD) r|=Robot_action::PICKUP;
		if(state.balls_held){
			r|=Robot_action::SHOOT1;
			r|=Robot_action::SHOOT2;
			r|=Robot_action::SHOOT3;
		}
		r|=Robot_action::DRIVE_TO_OWN_END;
	}else if(on_near_end(state.location)){
		if(ball_own_end && state.balls_held<MAX_BALLS_HELD) r|=Robot_action::PICKUP;
		r|=Robot_action::DRIVE_TO_SHOOTING_END;
	}else{
		r|=Robot_action::DRIVE_TO_OWN_END;
		r|=Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return r;
}

Robot_action rand_legal(Robot_state state,bool ball_shooting_end,bool ball_own_end){
	auto m=legal_actions(state,ball_shooting_end,ball_own_end);
	return choose(m);
}

struct Full_loader{
	//always try to get a full load before trying to shoot

	Robot_action operator()(Robot_state state,int ball_shooting_end,int ball_own_end){
		if(state.balls_held<5){
			if(at_shooting_end(state.location) && ball_shooting_end){
				return Robot_action::PICKUP;
			}
			if(on_near_end(state.location) && ball_own_end){
				return Robot_action::PICKUP;
			}
			return Robot_action::DRIVE_TO_OWN_END;
		}
		if(!at_shooting_end(state.location)){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::SHOOT3;
	}
};

struct Full_cycle{
	//always try to get a full load before trying to shoot
	//and always try to shoot all of the balls before trying to reload
	bool picking_up=1;

	Robot_action operator()(Robot_state state,int ball_shooting_end,int ball_own_end){
		if(picking_up && state.balls_held==MAX_BALLS_HELD){
			picking_up=0;
		}
		if(state.balls_held==0){
			picking_up=1;
		}

		if(picking_up){
			if(at_shooting_end(state.location) && ball_shooting_end){
				return Robot_action::PICKUP;
			}
			if(on_near_end(state.location) && ball_own_end){
				return Robot_action::PICKUP;
			}
			return Robot_action::DRIVE_TO_OWN_END;
		}
		if(!at_shooting_end(state.location)){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::SHOOT3;
	}
};

Robot_action always_cycle(Robot_state state,int ball_shooting_end,int ball_own_end){
	(void)ball_shooting_end;
	(void)ball_own_end;

	if(at_shooting_end(state.location)){
		if(state.balls_held){
			return Robot_action::SHOOT3;
		}
		return Robot_action::DRIVE_TO_OWN_END;
	}
	if(on_near_end(state.location)){
		if(state.balls_held==MAX_BALLS_HELD){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::PICKUP;
	}
	if(state.balls_held){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return Robot_action::DRIVE_TO_OWN_END;
}

Robot_action always_cycle1(Robot_state state,int ball_shooting_end,int ball_own_end){
	(void)ball_shooting_end;
	(void)ball_own_end;

	if(at_shooting_end(state.location)){
		if(state.balls_held){
			return Robot_action::SHOOT1;
		}
		return Robot_action::DRIVE_TO_OWN_END;
	}
	if(on_near_end(state.location)){
		if(state.balls_held==MAX_BALLS_HELD){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::PICKUP;
	}
	if(state.balls_held){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return Robot_action::DRIVE_TO_OWN_END;
}

Robot_action always_load(Robot_state state,int ball_shooting_end,int ball_own_end){
	(void)ball_shooting_end;
	(void)ball_own_end;

	if(at_shooting_end(state.location)){
		if(state.balls_held){
			return Robot_action::SHOOT3;
		}
		return Robot_action::DRIVE_TO_OWN_END;
	}
	if(on_near_end(state.location)){
		if(state.balls_held==MAX_BALLS_HELD){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::LOAD_STATION;
	}
	if(state.balls_held){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return Robot_action::DRIVE_TO_OWN_END;
}

Robot_action always_load1(Robot_state state,int ball_shooting_end,int ball_own_end){
	(void)ball_shooting_end;
	(void)ball_own_end;

	if(at_shooting_end(state.location)){
		if(state.balls_held){
			return Robot_action::SHOOT1;
		}
		return Robot_action::DRIVE_TO_OWN_END;
	}
	if(on_near_end(state.location)){
		if(state.balls_held==MAX_BALLS_HELD){
			return Robot_action::DRIVE_TO_SHOOTING_END;
		}
		return Robot_action::LOAD_STATION;
	}
	if(state.balls_held){
		return Robot_action::DRIVE_TO_SHOOTING_END;
	}
	return Robot_action::DRIVE_TO_OWN_END;
}

using Log=std::vector<std::map<Alliance_station,Robot_action>>;

template<typename Func,typename T>
vector<vector<T>> split_on(Func f,vector<T> const& a){
	vector<vector<T>> r;
	vector<T> here;
	for(auto elem:a){
		here|=elem;
		if(f(elem)){
			r.push_back(here);
			here.clear();
		}
	}
	return r;
}

template<typename T>
vector<pair<size_t,T>> uniq_c(vector<T> const& a){
	vector<pair<size_t,T>> r;
	for(auto elem:a){
		if(r.empty()){
			r|=make_pair(1,elem);
		}else{
			auto& last=r[r.size()-1];
			if(last.second==elem){
				last.first++;
			}else{
				r|=make_pair(1,elem);
			}
		}
	}
	return r;
}

bool shoot_action(Robot_action a){
	return a==Robot_action::SHOOT1 || a==Robot_action::SHOOT2 || a==Robot_action::SHOOT3;
}

void analyze_cycles(Log const& log){
	//print_lines(log);
	for(auto i:options((Alliance_station*)0)){
		auto v=mapf([=](auto x){ return x[i]; },log);
		auto sp=split_on(shoot_action,v);
		PRINT(i);
		print_lines(uniq_c(sp));
	}
}

pair<Log,int> run_match(Strategy strategy,bool verbose=0){
	vector<map<Alliance_station,Robot_action>> log;
	Game_state game{};
	
	static const int MATCH_LENGTH=135;//just doing teleop for now.

	if(verbose){
		//cout<<"t\t";
		cout<<"sred\tsblue\t";
		cout<<"dsred\tdsblue\t";
		cout<<"fbred\tfbblue\t";
		cout<<"balls held\t";
		cout<<"location\t";
		cout<<"\n";
	}

	for(auto i:range(0,MATCH_LENGTH,2)){
		(void)i;
		//PRINT(i);
		//PRINT(game);
		//cout<<i<<"\t";
		if(verbose){
			cout<<game.scores[Alliance::RED]<<"\t"<<game.scores[Alliance::BLUE]<<"\t";
			cout<<game.driver_station_balls[Alliance::RED]<<"\t";
			cout<<game.driver_station_balls[Alliance::BLUE]<<"\t";
			cout<<game.floor_balls[Alliance::RED]<<"\t";
			cout<<game.floor_balls[Alliance::BLUE]<<"\t";
			cout<<mapf([](auto x){ return x.balls_held; },values(game.robots))<<"\t";
			cout<<mapf([](auto x){ return x.location; },values(game.robots))<<"\t";
			cout<<"\n";
		}
		auto [chosen,g1]=next(game,strategy);
		log|=chosen;
		game=g1;
		assert(valid(game));
	}
	auto result=(int)game.scores[Alliance::RED]-(int)game.scores[Alliance::BLUE];
	//PRINT(result);
	if(fabs(result)>500)nyi
	return make_pair(log,result);
}

int clamp(int min,int max,int value){
	assert(min<=max);
	if(value<min) return min;
	if(value>max) return max;
	return value;
}

void tournament(){
	auto rand_action=[](auto,auto,auto){ return rand((Robot_action*)0); };
	map<string,Robot_strategy> strategies{
		{"rand_action",rand_action},
		{"rand_legal",rand_legal},
		{"choose_action",choose_action},
		{"Full_loader",Full_loader{}},
		{"Full_cycle",Full_cycle{}},
		{"always_cycle",always_cycle},
		{"always_load",always_load},
		{"ac1",always_cycle1},
		{"al1",always_load1}
	};

	//with 4 different robot types, there should be 4096 different matchups
	vector<vector<string>> match_strategies;
	for(auto a:strategies){
		for(auto b:strategies){
			for(auto c:strategies){
				auto alliance1=sorted(vector<string>{a.first,b.first,c.first});
				for(auto d:strategies){
					for(auto e:strategies){
						for(auto f:strategies){
							auto alliance2=sorted(vector<string>{d.first,e.first,f.first});
							vector<string> all=(alliance1+alliance2);
							match_strategies.push_back(all);
							/*vector<string> v{
								a.first,
								b.first,
								c.first,
								d.first,
								e.first,
								f.first
							};
							match_strategies.push_back(v);*/
						}
					}
				}
			}
		}
	}
	
	vector<int> result;
	map<string,vector<int>> result_by_strategy;
	map<vector<string>,vector<int>> result_by_alliance;
	for(auto x:match_strategies){
		Strategy s=to_map(zip(
			options((Alliance_station*)0),
			mapf([&](auto y){ return strategies[y]; },x)
		));
		auto [log,diff]=run_match(s);
		(void)log;
		result|=diff;
		//auto winning_alliance=(diff>0)?Alliance::RED:Alliance::BLUE;
		for(auto [loc,strat]:zip(options((Alliance_station*)0),x)){
			//result_by_strategy[strat]|=(winning_alliance==loc.alliance);
			result_by_strategy[strat]|=(loc.alliance==Alliance::RED)?diff:-diff;
		}
		result_by_alliance[take(3,x)]|=diff;
		result_by_alliance[skip(3,x)]|=-diff;
	}
	PRINT(result.size());
	PRINT(min(result));
	PRINT(max(result));
	PRINT(mean(result));
	//float played_each=4096.0*6/4;
	//PRINT(played_each);
	//print_lines(result_by_strategy);
	for(auto [name,results]:result_by_strategy){
		cout<<name<<":"<<results.size()<<" "<<mean(results)<<"\n";
	}

	for(
		auto [a,results]:
		sort_by(
			[](auto x){
				return -mean(
					mapf(
						[](auto y)->float{ return clamp(-1,1,y); },
						x.second
					)
				);
			},
			to_vec(result_by_alliance)
		)
	){
		//cout<<int(100*mean(mapf([](auto x)->float{ return clamp(-1,1,x); },results)));
		cout<<int(mean(mapf([](auto x)->float{ return clamp(-30,30,x); },results)));
		//auto win=filter([](auto x){ return x>0; },results).size();
		//auto loss=filter([](auto x){ return x<0; },results).size();
		//auto tie=filter([](auto x){ return x==0; },results).size();
		//cout<<win<<"-"<<loss<<"-"<<tie<<"\t";
		cout<<"\t"<<a<<":"<<results.size()<<" "<<mean(results)<<"\n";
	}
}

/*int main1(){
	Game_state game{};
	Strategy strategy;

	for(auto i:options((Alliance_station*)0)){
		strategy[i]=choose_action;//rand_legal;//rand_action;//choose_action;
	}
	strategy[Alliance_station{Alliance::RED,Alliance_station_number{3}}]=Full_loader{};
	
	static const int MATCH_LENGTH=135;//just doing teleop for now.

	//cout<<"t\t";
	cout<<"sred\tsblue\t";
	cout<<"dsred\tdsblue\t";
	cout<<"fbred\tfbblue\t";
	cout<<"balls held\t";
	cout<<"location\t";
	cout<<"\n";

	for(auto i:range(0,MATCH_LENGTH,2)){
		(void)i;
		//PRINT(i);
		//PRINT(game);
		//cout<<i<<"\t";
		cout<<game.scores[Alliance::RED]<<"\t"<<game.scores[Alliance::BLUE]<<"\t";
		cout<<game.driver_station_balls[Alliance::RED]<<"\t";
		cout<<game.driver_station_balls[Alliance::BLUE]<<"\t";
		cout<<game.floor_balls[Alliance::RED]<<"\t";
		cout<<game.floor_balls[Alliance::BLUE]<<"\t";
		cout<<mapf([](auto x){ return x.balls_held; },values(game.robots))<<"\t";
		cout<<mapf([](auto x){ return x.location; },values(game.robots))<<"\t";
		cout<<"\n";
		game=next(game,strategy);
		assert(valid(game));
	}
	return 0;
}*/

int main(){
	/*
	 * Enumerating what the possible sane strategies could be:
	 * 1) if had balls on the field that your opponents are going to use, always want to pick them up.
	 * */
	tournament();

	Alliance_station R1{Alliance::RED,Alliance_station_number{1}};
	Alliance_station R2{Alliance::RED,Alliance_station_number{2}};
	Alliance_station R3{Alliance::RED,Alliance_station_number{3}};
	Alliance_station B1{Alliance::BLUE,Alliance_station_number{1}};
	Alliance_station B2{Alliance::BLUE,Alliance_station_number{2}};
	Alliance_station B3{Alliance::BLUE,Alliance_station_number{3}};

	auto [log,diff]=run_match(
		Strategy{
			{R1,always_cycle},
			{R2,rand_legal},
			{R3,choose_action},
			{B1,Full_loader{}},
			{B2,Full_cycle{}},
			{B3,always_cycle}
		},
		1
	);
	PRINT(diff);
	analyze_cycles(log);

	/*
	 * interesting questions to ask about cycles:
	 *how many did the make?
	 what was the distribution of balls shot in each cycle?
	 how many of the cycles went across vs. just stayed on a side?
	*/
}
