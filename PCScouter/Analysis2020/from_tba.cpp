#include<fstream>
#include "../tba/db.h"
#include "../tba/tba.h"
#include "capabilities.h"
#include "util.h"
#include "game.h"

using namespace std;

//start generic code

//start program-specific code

void print_r(unsigned indent,tba::Match const& a){
	nyi
	PRINT(indent);
	PRINT(a);
	/*indent_by(indent);
	indent++;
	cout<<"Match\n";
	#define X(A,B) print_r(indent,a.B);
	TBA_MATCH(X)
	#undef X*/
}

void print_r(unsigned indent,tba::Match_Score_Breakdown_2020_Alliance const& a){
	indent_by(indent);
	indent++;
	cout<<"Match_Score_Breakdown_2020_Alliance\n";
	#define X(A,B) indent_by(indent); cout<<""#B<<"("#A<<")\n"; print_r(indent+1,a.B);
	TBA_MATCH_SCORE_BREAKDOWN_2020_ALLIANCE(X)
	#undef X
}

void print_r(unsigned indent,tba::Match_Score_Breakdown_2020 const& a){
	indent_by(indent);
	indent++;
	cout<<"Match_Score_Breakdown_2020\n";
	#define X(A,B) indent_by(indent); cout<<""#B<<"\n"; print_r(indent+1,a.B);
	TBA_MATCH_SCORE_BREAKDOWN_2020(X)
	#undef X
}

template<typename A,typename B,typename C,typename D,typename E>
void print_r(unsigned indent,std::variant<A,B,C,D,E> const& a){
	if(std::holds_alternative<A>(a)) return print_r(indent,std::get<A>(a));
	if(std::holds_alternative<B>(a)) return print_r(indent,std::get<B>(a));
	if(std::holds_alternative<C>(a)) return print_r(indent,std::get<C>(a));
	if(std::holds_alternative<D>(a)) return print_r(indent,std::get<D>(a));
	if(std::holds_alternative<E>(a)) return print_r(indent,std::get<E>(a));
	assert(0);
}

template<typename A,typename B,typename C,typename D>
void print_r(unsigned,std::variant<A,B,C,D> const&)nyi

template<typename T>
void print_r(unsigned i,std::optional<T> const& a){
	if(a){
		return print_r(i,*a);
	}
	indent_by(i);
	cout<<"NULL\n";
}

#define INFO_ITEMS(X)\
	X(multiset<tba::Init_line>,init_line)\
	X(vector<unsigned>,auto_low)\
	X(vector<unsigned>,auto_outer)\
	X(vector<unsigned>,auto_inner)\
	X(vector<unsigned>,tele_low)\
	X(vector<unsigned>,tele_outer)\
	X(vector<unsigned>,tele_inner)\
	X(multiset<bool>,wheel_spin)\
	X(multiset<bool>,wheel_color)\
	X(multiset<tba::Endgame>,endgame)\
	X(multiset<bool>,balance)

struct Info{
	INFO_ITEMS(INST)
};

std::ostream& operator<<(std::ostream& o,Info const& ){
	o<<"Info(...)";
	return o;
}

void print_r(unsigned indent,Info const& a){
	indent_by(indent);
	indent++;
	cout<<"Info\n";
	#define X(A,B) print_r(indent,a.B);
	INFO_ITEMS(X)
	#undef X
}

template<typename T>
std::vector<T> operator/(std::vector<T> a,int x){
	for(auto& elem:a){
		elem/=x;
	}
	return a;
}

Dist dist_third(vector<unsigned> const& a,vector<unsigned> const& b,vector<unsigned> const& c){
	auto totals=MAP(sum,zip(a,b,c))/3;//note that the fact that these are all rounded down introduces some bias.
	Dist r;
	r.clear();
	for(auto elem:totals){
		r[elem]+=1.0/totals.size();
	}
	return r;
}

multiset<unsigned>& operator|=(multiset<unsigned>& a,int b){
	assert(b>=0);
	a.insert(b);
	return a;
}

Team parse_team(tba::Team_key const& a){
	auto s=a.str();
	assert(s.size()>=4);
	assert(s[0]=='f');
	assert(s[1]=='r');
	assert(s[2]=='c');
	return Team{atoi(s.c_str()+3)};
}

void help(){
	cout<<"Arguments:\n";
	cout<<"--event <EVENT_KEY>\n";
	cout<<"\tMandatory - the event code.  For example, \"2020isde1\".\n";
	cout<<"--team <TEAM_NUMBER>\n";
	cout<<"\tDefaults to 1425.\n";
	cout<<"--help\n";
	cout<<"\tShow this message.\n";
	exit(0);
}

struct Args{
	Team team;
	tba::Event_key event;
};

Args parse_args(int argc,char **argv){
	(void)argc;
	Team team{1425};
	optional<tba::Event_key> event;
	for(auto at=argv+1;*at;++at){
		if(string(*at)=="--team"){
			at++;
			assert(at);
			team=Team{atoi(*at)};
		}else if(string(*at)=="--event"){
			at++;
			assert(at);
			event=tba::Event_key{*at};
		}else if(string(*at)=="--help"){
			help();
		}else{
			cout<<"Unrecognized argument.\n";
			help();
		}
	}
	if(!event){
		cout<<"Error: Valid event must be specified.\n";
		exit(1);
	}
	return Args{team,*event};
}

int main1(int argc,char **argv){
	Args args=parse_args(argc,argv);
	std::ifstream f("../tba/auth_key");
	std::string tba_key;
	getline(f,tba_key);
	tba::Cached_fetcher cf{tba::Fetcher{tba::Nonempty_string{tba_key}},tba::Cache{}};
	//tba::Event_key event_key{"2020isde1"};
	tba::Event_key event_key{args.event};
	auto found=event_matches(cf,event_key);
	//PRINT(found.size());
	{
		auto f=filter([](auto x){ return x.score_breakdown; },found);
		cout<<"Matches found:"<<found.size()<<"\n";
		cout<<"Score breakdowns:"<<f.size()<<"\n";
		if(found.size()){
			cout<<"fraction found:"<<(0.0+f.size())/found.size()<<"\n";
		}
	}
	//print_r(found);

	map<tba::Team_key,Info> info;

	//map<Input_row> rows;
	for(auto match:found){
		/*#define X(A,B) cout<<""#B<<"("#A<<")"<<"\n"; print_r(1,match.B);
		TBA_MATCH(X)
		#undef X*/
		//nyi
		
		if(!match.score_breakdown) continue;
		/*for(auto [a,b]:zip(match.alliances,*match.score_breakdown)){
			PRINT(a);
			PRINT(b);
			nyi
		}*/

		auto analyze_alliance=[&](
			tba::Match_Alliance const& alliance,
			tba::Match_Score_Breakdown_2020_Alliance const& result
		){
			//print_r(0,alliance);
			//print_r(0,result);

			assert(alliance.team_keys.size()==3);

			auto& t1=info[alliance.team_keys[0]];
			auto& t2=info[alliance.team_keys[1]];
			auto& t3=info[alliance.team_keys[2]];

			t1.init_line|=result.initLineRobot1;
			t2.init_line|=result.initLineRobot2;
			t3.init_line|=result.initLineRobot3;

			t1.endgame|=result.endgameRobot1;
			t2.endgame|=result.endgameRobot2;
			t3.endgame|=result.endgameRobot3;

			vector<Info*> teams;
			teams|=&t1;
			teams|=&t2;
			teams|=&t3;

			for(auto t:teams){
				assert(t);
				t->auto_low|=result.autoCellsBottom;
				t->auto_outer|=result.autoCellsOuter;
				t->auto_inner|=result.autoCellsInner;
				t->tele_low|=result.teleopCellsBottom;
				t->tele_outer|=result.teleopCellsOuter;
				t->tele_inner|=result.teleopCellsInner;
				t->wheel_spin|=result.stage2Activated;
				t->wheel_color|=result.stage3Activated;
				t->balance|=(result.endgameRungIsLevel==tba::Rung_level::IsLevel);
			}
			/*auto auto_balls_scored=result.autoCellsBottom+result.autoCellsOuter+result.autoCellsInner;;
			auto teleop_balls_scored=result.teleopCellsBottom+result.teleopCellsOuter+result.teleopCellsInner;

			//this is only the number that could be towards advancing levels
			auto balls_scored=min(9,auto_balls_scored)+teleop_balls_scored;
			t1.alliance_balls_scored|=balls_scored;
			t2.alliance_balls_scored|=balls_scored;
			t3.alliance_balls_scored|=balls_scored;*/
		};

		analyze_alliance(
			match.alliances.red,
			get<tba::Match_Score_Breakdown_2020>(*match.score_breakdown).red
		);
		analyze_alliance(
			match.alliances.blue,
			get<tba::Match_Score_Breakdown_2020>(*match.score_breakdown).blue
		);
	}

	//PRINT(info.size());
	//print_r(info);

	auto m=to_map(mapf(
		[](auto p)->std::pair<Team,Robot_capabilities>{
			auto [team,data]=p;
			//PRINT(team);
			//PRINT(data);
			auto cap=Robot_capabilities{
				mean(mapf([](auto x){ return x==tba::Init_line::Exited; },data.init_line)),
				mean_d(data.auto_low)/3,
				mean_d(data.auto_outer)/3,
				mean_d(data.auto_inner)/3,
				mean_d(data.tele_low)/3,
				mean_d(data.tele_outer)/3,
				mean_d(data.tele_inner)/3,
				dist_third(data.auto_low,data.auto_outer,data.auto_inner),
				dist_third(data.tele_low,data.tele_outer,data.tele_inner),
				mean(data.wheel_spin),
				mean(data.wheel_color),

				//climbed self
				mean(mapf([](auto x){ return x==tba::Endgame::Hang; },data.endgame)),

				0, //assist2
				0, //assist1
				0, //climb was assisted

				mean(mapf([](auto x){ return x==tba::Endgame::Park; },data.endgame)),
				mean(data.balance)
			};
			return make_pair(parse_team(team),cap);
		},
		info
	));

	//print_lines(m);

	//now show the extemes for each of the values
	#define X(A,B) {\
		auto data=mapf([](auto x){ return x.B; },values(m));\
		cout<<""#B<<":"<<min(data)<<" "<<max(data)<<" "<<mean(data)<<"\n";\
	}
	ROBOT_CAPABILITIES(X)
	#undef X
	show(m);
	auto list=make_picklist(args.team,m);
	show_picklist(args.team,list);
	return 0;
}

int main(int argc,char **argv){
	try{
		return main1(argc,argv);
	}catch(std::string const& s){
		std::cout<<"Caught: "<<s<<"\n";
	}
}
