#include<iostream>
#include<cmath>
#include<functional>
#include<unistd.h>
#include<ctype.h>
#include "../tba/data.h"
#include "input_data.h"
#include "../tba/db.h"
#include "../tba/tba.h"

using namespace std;

//start generic code

template<typename T>
T median(std::vector<T> const& a){
	assert(a.size());
	auto b=sorted(a);
	return b[b.size()/2];
}

template<typename T>
vector<T> flatten(std::vector<std::vector<T>> const& a){
	vector<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

template<typename K,typename V>
vector<V> seconds(map<K,V> const& a){
	return mapf([](auto p){ return p.second; },a);
}

template<typename Func>
std::map<std::pair<int,int>,double> normalize(Func f){
	std::map<std::pair<int,int>,size_t> m;
	for(auto x:range(52)){
		for(auto y:range(27)){
			m[std::make_pair(x,y)]=f(x,y);
		}
	}
	auto total=sum(seconds(m));
	return map_values(
		[=](auto v){
			return (0.0+v)/total;
		},
		m
	);
}

template<typename Func,typename T>
size_t count(Func f,T const& t){
	size_t r=0;
	for(auto elem:t){
		if(f(elem)){
			r++;
		}
	}
	return r;
}

template<typename T>
std::vector<std::pair<T,T>> pick_stride(unsigned stride,std::vector<T> const& a){
	//get all of the combinations that are a certain distance from each other.
	std::vector<std::pair<T,T>> r;
	for(size_t i=0;i+stride<a.size();i++){
		r|=make_pair(a[i],a[i+stride]);
	}
	return r;
}

template<typename A,typename B>
B max_seconds(std::vector<std::pair<A,B>> const& a){
	assert(a.size());
	B r=a[0].second;
	for(auto const& elem:a){
		r=max(r,elem.second);
	}
	return r;
}

template<typename Func>
auto mapf(Func f,std::string const& s){
	vector<ELEM(s)> r;
	for(auto elem:s){
		r|=f(elem);
	}
	return r;
}

string str(vector<char> v){
	stringstream ss;
	for(auto c:v){
		ss<<c;
	}
	return ss.str();
}

string to_caps(string const& s){
	//MAP(toupper,s);
	return str(mapf([](auto c)->char{ return toupper(c); },s));
}

template<typename T>
vector<optional<T>> to_optionals(vector<T> const& a){
	return mapf([](auto x){ return std::optional<T>{x}; },a);
}

//start program-specific code

#define PERIOD_OPTIONS(X)\
	X(AUTO)\
	X(TELE_MAIN)\
	X(ENDGAME)

enum class Period{
	#define X(A) A,
	PERIOD_OPTIONS(X)
	#undef X
};

std::ostream& operator<<(std::ostream& o,Period a){
	#define X(A) if(a==Period::A) return o<<""#A;
	PERIOD_OPTIONS(X)
	#undef X
	assert(0);
}

std::vector<Period> options(Period const*){
	return vector{
		#define X(A) Period::A,
		PERIOD_OPTIONS(X)
		#undef X
	};
}

vector<Period> periods(std::optional<Period> const& a){
	if(a) return {*a};
	return options((Period*)0);
}

using Time=double;//# of seconds

bool in_period(Time t,Period p){
	switch(p){
		case Period::AUTO:
			return t>=0 && t<=15;
		case Period::TELE_MAIN:
			//for our purposes, we're saying endgame is last 20 seconds.
			return t>=15 && t<=150-20;
		case Period::ENDGAME:
			return t>=150-20;
		default:
			assert(0);
	}
}

void plot(std::vector<tba::Zebra_team> const& data,string name="all"){
	//This is going to feed the data out to python for the plotting.
	string plotter_name="plotter_"+name+"_line.py";
	{
		ofstream f(plotter_name.c_str());
		f<<"import numpy as np\n";
		f<<"import matplotlib.pyplot as plt\n";
		//f<<"#a:"<<a<<"\n";
		for(auto a:data){
			f<<"xs=[]\n";
			auto python_literal=[](optional<double> x)->string{
				if(!x){
					return "None";
				}
				return as_string(x);
			};
			for(auto elem:a.xs){
				f<<"xs.append("<<python_literal(elem)<<")\n";
			}
			f<<"ys=[]\n";
			for(auto elem:a.ys){
				f<<"ys.append("<<python_literal(elem)<<")\n";
			}
			f<<"plt.plot(xs,ys)\n";
		}

		f<<"plt.savefig(\"line_"<<name<<".png\")\n";
	}

	pid_t p=fork();
	if(p==0){
		auto r=system( ("python "+plotter_name).c_str() );
		assert(r==0);
		exit(0);
	}
}

void plot(tba::Zebra_team const& a,string name){
	//This is going to feed the data out to python for the plotting.
	pid_t p=fork();
	if(p!=0) return;

	string py_name=[=](){
		stringstream ss;
		ss<<"plotter_"<<getpid()<<".py";
		return ss.str();
	}();

	{
		ofstream f(py_name);
		f<<"import numpy as np\n";
		f<<"import matplotlib.pyplot as plt\n";
		//f<<"#a:"<<a<<"\n";
		f<<"xs=[]\n";
		auto python_literal=[](optional<double> x)->string{
			if(!x){
				return "None";
			}
			return as_string(x);
		};
		for(auto elem:a.xs){
			f<<"xs.append("<<python_literal(elem)<<")\n";
		}
		f<<"ys=[]\n";
		for(auto elem:a.ys){
			f<<"ys.append("<<python_literal(elem)<<")\n";
		}
		f<<"plt.plot(xs,ys)\n";
		f<<"plt.savefig(\"fig_"<<name<<".png\")\n";
	}

	auto r=system( ("python "+py_name).c_str() );
	assert(r==0);
	exit(0);
}

tba::Team_key to_team_key(Team const& a){
	stringstream ss;
	ss<<"frc"<<a;
	return tba::Team_key{ss.str()};
}

bool operator==(tba::Team_key const& a,Team b){
	return a==to_team_key(b);
}

using Range=std::pair<double,double>;

std::pair<Range,Range> limits(std::vector<tba::Zebra_team> const& v){
	auto xs=non_null(flatten(mapf([](auto a){ return a.xs; },v)));
	auto ys=non_null(flatten(mapf([](auto a){ return a.ys; },v)));
	return make_pair(
		make_pair(min(xs),max(xs)),
		make_pair(min(ys),max(ys))
	);
}

using Point=std::pair<double,double>;
using Datapoint=std::pair<Time,Point>;

void heatmap(std::function<double(int,int)> density,std::string name){
	auto p=fork();
	if(p!=0) return;

	auto plotfile="plot_heat_"+name+".py";
	{
		ofstream f(plotfile);
		f<<"import numpy as np\n";
		f<<"import matplotlib\n";
		f<<"import matplotlib.pyplot as plt\n";
		f<<"ylabels=[]\n";
		f<<"xlabels=[]\n";
		static const int WIDTH=53;
		static const int HEIGHT=30;
		for(auto y:range(HEIGHT)){
			f<<"ylabels.append("<<27-y<<")\n";
		}
		for(auto x:range(WIDTH)){
			f<<"xlabels.append("<<x<<")\n";
		}
		f<<"data=[\n";
		for(auto y:range(HEIGHT)){
			f<<"\t[";
			for(auto x:range(WIDTH)){
				//auto v=min(50,density(x,27-y));
				auto v=density(x,27-y);
				f<<v<<",";
			}
			f<<"\t],\n";
		}
		f<<"\t]\n";
		f<<"fig,ax=plt.subplots()\n";
		f<<"im=ax.imshow(data)\n";
		f<<"ax.set_xticks(np.arange("<<WIDTH<<"))\n";
		f<<"ax.set_yticks(np.arange("<<HEIGHT<<"))\n";
		f<<"ax.set_xticklabels(xlabels)\n";
		f<<"ax.set_yticklabels(ylabels)\n";
		f<<"fig.tight_layout()\n";
		f<<"plt.savefig(\"heat_"<<name<<".png\")\n";
	}
	auto r=system((string()+"python "+plotfile).c_str());
	assert(r==0);
	exit(0);
}

void largest_jump(std::vector<double> const& time,tba::Zebra_team const& a,std::string const& name){
	assert(time.size()==a.xs.size());
	assert(time.size()==a.ys.size());

	vector<Datapoint> data;
	for(auto [t,x,y]:zip(time,a.xs,a.ys)){
		if(x){
			assert(y);
			data|=make_pair(t,make_pair(*x,*y));
		}else{
			assert(!y);
		}
	}

	map<pair<int,int>,unsigned> buckets;
	for(auto elem:data){
		buckets[make_pair(elem.second.first,elem.second.second)]++;
	}
	PRINT(buckets.size());
	heatmap(
		[&](int x,int y)->double{
			/*return filter(
				[=](auto elem){ return int(elem.second.first)==x && int(elem.second.second)==y; },
				data
			).size();*/
			return buckets[make_pair(x,y)];
		},
		name
	);

	optional<Datapoint> previous;
	vector<double> instant_speeds;
	for(auto elem:data){
		if(!previous){
			previous=elem;
			continue;
		}
		auto dt=elem.first-previous->first;
		assert(dt>0);
		auto dx=elem.second.first-previous->second.first;
		auto dy=elem.second.second-previous->second.second;
		auto dist=sqrt(dx*dx+dy*dy);
		auto speed=dist/dt;
		instant_speeds|=speed;
		previous=elem;
	}

	//interesting to know the shortest time to go distance x
	//make list of distances (every 5 feet?) and then go through increasing timescales till they all are filled in
	//double threshold=10;
	//going to just do this brute-force to start
	vector<pair<Datapoint,Datapoint>> r;
	//vector<pair<Point,Point>> r2;
	for(auto stride:range(size_t(1),data.size())){
		r|=pick_stride(stride,data);
	}
	vector<double> ts;
	vector<double> x1;
	vector<double> y1;

	for(auto i:range(data.size())){
		for(auto j:range(i,data.size())){
			auto p1=data[i];
			auto p2=data[j];
			ts|=p2.first-p1.first;
			x1|=p2.second.first-p1.second.first;
			y1|=p2.second.second-p2.second.second;
		}
	}

	using Distance=double;
	using Speed=double;
	vector<pair<Distance,Speed>> v;
	for(auto x:r){
		//PRINT(x);
		auto dt=x.second.first-x.first.first;
		assert(dt>0);
		auto dx=x.first.second.first-x.second.second.first;
		auto dy=x.first.second.second-x.second.second.second;
		auto dist=sqrt(dx*dx+dy*dy);
		//assert(dist>=0);
		//PRINT(dist);
		//PRINT(dt);
		auto speed=dist/dt;
		//assert(speed<=max(instant_speeds));
		v|=make_pair(dist,speed);
	}
	cout<<"\n"<<name<<"\n";
	cout<<"Distance (ft)\tSpeed (ft/s)\tTime (s)\n";
	auto g=group(
		[](auto x){
			(void)x;
			return 0;//int(x.first/5);
		},
		v
	);
	PRINT(g.size());
	for(auto dist:range(0,60,5)){
		auto m=filter([=](auto x){ return x.first>=dist && x.first<=dist+5; },v);
		if(m.size()){
			auto speed=max_seconds(m); //max(seconds(m));
			assert(speed>0);
			cout<<dist<<"\t\t"<<speed<<"\t\t"<<dist/speed<<"\n";
		}
	}

	//could make a histogram of these
	/*PRINT(min(instant_speeds));
	PRINT(max(instant_speeds));
	PRINT(mean(instant_speeds));
	PRINT(median(instant_speeds));*/
	//return max(instant_speeds);
}

tba::Zebra_team flip_field(tba::Zebra_team a){
	a.xs=mapf(
		[](auto x)->optional<double>{
			if(x){
				return 52+5.25/12-*x;
			}
			return x;
		},
		a.xs
	);
	a.ys=mapf(
		[](auto y)->optional<double>{
			if(y){
				return 26+11.25/12-*y;
			}
			return y;
		},
		a.ys
	);
	return a;
}

vector<tba::Zebra_team> all_teams(tba::Zebra const& a){
	return a.alliances.red|a.alliances.blue;
}

tba::Zebra_team find_team(Team team,tba::Zebra const& data){
	for(auto x:data.alliances.red){
		if(x.team_key==team){
			return x;
		}
	}
	for(auto x:data.alliances.blue){
		if(x.team_key==team){
			//Take advantage of the rotational symmetry to normalize to being red.
			return flip_field(x);
		}
	}
	assert(0);
}

Team to_team(tba::Team_key a){
	return Team{atoi(a.str().c_str()+3)};
}

bool in_own_trench(Point p){
	auto [x,y]=p;
	return x>=19 && x<=23 && y>=0 && y<=5;
}

bool in_own_trench_entrance(Point p){
	auto [x,y]=p;
	return x>=19 && x<21 && y>=0 && y<=5;
}

bool in_own_trench_exit(Point p){
	auto [x,y]=p;
	return x>=21 && x<=23 && y>=0 && y<=5;
}

vector<Point> to_points(tba::Zebra_team const& a){
	vector<Point> r;
	for(auto [x,y]:zip(a.xs,a.ys)){
		if(x){
			assert(y);
			r|=make_pair(*x,*y);
		}
	}
	return r;
}

map<pair<int,int>,unsigned> layout(){
	nyi
}

auto YEAR=tba::Year{2020};

static const int WIDTH=52;
static const int HEIGHT=30;

//using Heatmap=std::array<std::array<int,WIDTH>,HEIGHT>;

template<typename Func,typename K,typename V>
auto map_keys(Func f,std::map<K,V> const& a){
	std::map<K,decltype(f(a.begin()->second))> r;
	for(auto [k,v]:a){
		r[k]=f(v);
	}
	return r;
}

template<typename K,typename V>
map<K,V> operator-(std::map<K,V> const& a,std::map<K,V> const& b){
	map<K,V> r;
	for(auto k:keys(a)|keys(b)){
		auto get=[k](std::map<K,V> const& m){
			auto f=m.find(k);
			if(f==m.end()) return V{};
			return f->second;
		};
		r[k]=get(a)-get(b);
	}
	return r;
}

void heatmap(std::map<pair<int,int>,double> const& data,std::string const& name){
	heatmap(
		[&](int x,int y){
			auto pt=make_pair(x,y);
			auto f=data.find(pt);
			if(f==data.end()){
				return 0.0;
			}
			return f->second;
		},
		name
	);
}

void study_deviation(tba::Cached_fetcher &f,Period period){
	auto e=event_teams_keys(f,tba::Event_key{"2020orwil"});
	using M=map<pair<int,int>,size_t>;
	M found;
	map<Team,M> by_team;
	for(auto team:e){
		for(auto match_key:team_matches_year_keys(f,team,YEAR)){
			auto data=zebra_motionworks(f,match_key);
			if(data){
				auto path=find_team(to_team(team),*data);
				for(auto [t,x,y]:zip(data->times,path.xs,path.ys)){
					if(in_period(t,period) && x){
						auto pt=make_pair(int(*x),int(*y));
						found[pt]++;
						by_team[to_team(team)][pt]++;
					}
				}
			}
		}
	}
	heatmap([&](int x,int y){ return found[make_pair(x,y)]; },"all");
	
	auto n=normalize(
		[&](int x,int y){ return found[make_pair(x,y)]; }
	);

	//first, just consider each individual spot as totally independent
	//and just sum up the difference for each of them
	//would get cleaner results if were doing some sort standard filter over an area
	auto by_team_n=map_keys(
		[&](M m){
			return normalize([&](int x,int y){ return m[make_pair(x,y)]; });
		},
		by_team
	);

	for(auto [team,tn]:by_team_n){
		auto diff=tn-n;
		heatmap(diff,"diff_"+as_string(team)+"_"+as_string(period));
	}
}

void study_deviation(tba::Cached_fetcher &f){
	for(auto period:options((Period*)0)){
		study_deviation(f,period);
	}
}

template<typename T>
std::vector<std::pair<T,T>> adjacent_pairs(std::vector<T> const& a){
	std::vector<std::pair<T,T>> r;
	if(a.size()){
		for(auto i:range(a.size()-1)){
			r|=std::make_pair(a[i],a[i+1]);
		}
	}
	return r;
}

int find_thru(vector<Point> const& a){
	return count(
		[](auto p){
			/*auto a=in_own_trench(p.first);
			auto a1=in_own_trench_entrance(p.first);
			auto a2=in_own_trench_exit(p.first);
			assert(!(a1 && a2));
			if(a){
				if( !(a1 || a2) ){
					PRINT(a);
					PRINT(a1);
					PRINT(a2);
				}
				assert(a1 || a2);
			}else{
				assert(!a1 && !a2);
			}

			auto b=in_own_trench(p.second);
			auto b1=in_own_trench_entrance(p.second);
			auto b2=in_own_trench_exit(p.second);
			*/
			//if(a1 && b2)nyi
			//if(a2 && b1)nyi

			//(void)b;
			//if(a || b)nyi
			auto fwd=in_own_trench_entrance(p.first) && in_own_trench_exit(p.second);
			auto rev=in_own_trench_entrance(p.second) && in_own_trench_exit(p.first);
			//if(fwd)nyi
			//if(rev)nyi
			return fwd || rev;
		},
		adjacent_pairs(a)
	);
}

template<typename K,typename V>
std::vector<std::pair<K,V>> to_pairs(std::map<K,V> const& a){
	std::vector<std::pair<K,V>> r;
	for(auto p:a) r|=p;
	return r;
}

void find_tall_bots(tba::Cached_fetcher &f){
	//initial algorithm:
	//see if in certain region of the field
	//advanced algorithm:
	//see when they actually make it through, rather than getting stuck in there
	//because we pushed them in
	//
	//interesting experiment:
	//look at the overall heat map of all robots
	//and highlight how robots deviate from that
	//
	//look at know tall bots and see where they don't go ever
	//
	//try to get direction of travel info?
	//to try to say who is being defended, or to say 
	
	auto e=event_teams_keys(f,tba::Event_key{"2020orwil"});
	map<Team,double> m;
	map<Team,double> thru_by_team;//per match

	for(auto team:e){
		std::vector<tba::Zebra_team> paths;
		static const auto YEAR=tba::Year{2020};
		for(auto match_key:team_matches_year_keys(f,team,YEAR)){
			auto data=zebra_motionworks(f,match_key);
			if(data){
				paths|=find_team(to_team(team),*data);
			}
		}
		auto pts=flatten(mapf(to_points,paths));
		auto c=count(in_own_trench,pts);

		if(pts.size()){
			int thrus=find_thru(pts);
			thru_by_team[to_team(team)]=(0.0+thrus)/paths.size();
			static const int SHOW_MATCHES=0;
			if(thrus && SHOW_MATCHES){
				for(auto match_key:team_matches_year_keys(f,team,YEAR)){
					auto data=zebra_motionworks(f,match_key);
					if(data){
						auto path=find_team(to_team(team),*data);
						int thrus=find_thru(to_points(path));
						if(thrus){
							cout<<team<<"\t"<<match_key<<"\t"<<thrus<<"\n";
						}
					}
				}
			}
		}

		/*PRINT(team);
		PRINT(c);
		PRINT(pts.size());*/
		if(pts.size()){
			m[to_team(team)]=(0.0+c)/pts.size();
		}
	}
	//print_lines(m);
	//print_lines(sorted(swap_pairs(to_vec(m))));
	//nyi
	cout<<"Number of times under own trench per match:\n";
	print_lines(sorted(swap_pairs(to_pairs(thru_by_team))));	
}

#define DISPLAY_STYLE(X)\
	X(LINE)\
	X(HEAT)

enum class Display_style{
	#define X(A) A,
	DISPLAY_STYLE(X)
	#undef X
};

std::ostream& operator<<(std::ostream& o,Display_style a){
	#define X(A) if(a==Display_style::A) return o<<""#A;
	DISPLAY_STYLE(X)
	#undef X
	assert(0);
}

std::vector<Display_style> options(Display_style const*){
	return {
		#define X(A) Display_style::A,
		DISPLAY_STYLE(X)
		#undef X
	};
}

tba::Event_key parse(char const *s,tba::Event_key const*){
	return tba::Event_key{s};
}

vector<tba::Event_key> options(tba::Event_key const*){
	//obviously, this is just a couple examples, no exhaustive
	return {
		tba::Event_key{"2020orore"},
		tba::Event_key{"2020orwil"}
	};
}

#define ARGS_ITEMS(X)\
	X(optional<Team>,team)\
	X(optional<tba::Match_key>,match)\
	X(optional<Display_style>,style)\
	X(optional<Period>,period)\
	X(optional<tba::Event_key>,event)\
	X(bool,demo)\
	X(bool,heat_diff)\

struct Args{
	ARGS_ITEMS(INST)
};

std::ostream& operator<<(std::ostream& o,Args const& a){
	o<<"Args(";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ARGS_ITEMS(X)
	#undef X
	return o<<")";
}

Team parse(string,Team const*)nyi

Team parse(const char *s,Team const*){
	return Team{atoi(s)};
}

tba::Match_key parse(const char *s,tba::Match_key const*){
	return tba::Match_key{s};
}

Display_style parse(const char *s,Display_style const* x){
	for(auto a:options(x)){
		if(s==as_string(a)){
			return a;
		}
	}
	assert(0);
}

Period parse(const char *s,Period const *x){
	for(auto a:options(x)){
		if(s==as_string(a)){
			return a;
		}
	}
	stringstream ss;
	ss<<"Invalid period: \""<<s<<"\"";
	throw ss.str();
}

template<typename T>
T parse(char *s,optional<T> const*){
	return parse(s,(T*)0);
}

vector<tba::Match_key> options(tba::Match_key const*){
	//these are obviously just examples, not a complete list
	return {tba::Match_key{"2020orore_qm4"},tba::Match_key{"2020orwil_sf1m2"}};
}

template<typename T>
vector<T> options(optional<T> const*){
	return options((T*)0);
}

vector<bool> options(bool const*){
	return {0,1};
}

bool parse(char const *s,bool const*){
	return atoi(s);
}

void help(){
	cout<<"Arguments:\n";
	#define X(A,B) {\
		cout<<"--"#B<<" <"<<to_caps(""#B)<<">\n";\
		cout<<"\tEx: "<<take(5,options((A*)0))<<"\n";\
	}
	ARGS_ITEMS(X)
	#undef X
	exit(0);
}

Args parse_args(int argc,char **argv){
	(void)argc;
	Args r{};
	for(int i=1;argv[i];i++){
		#define X(A,B) if(argv[i]==string("--"#B)){\
			i++;\
			assert(argv[i]);\
			r.B=parse(argv[i],(A*)0);\
			continue;\
		}
		ARGS_ITEMS(X)
		#undef X
		if(string(argv[i])=="--help"){
			help();
		}
		cout<<"Unrecognized arg:"<<argv[i]<<"\n";
		exit(1);
	}
	if(r.match && r.event){
		cout<<"Error: Match number and event may not both be specified\n";
		exit(1);
	}

	return r;
}

vector<Display_style> styles(std::optional<Display_style> const& a){
	if(a) return {*a};
	return options((Display_style*)0);
}

tba::Zebra_team to_zebra_team(vector<pair<double,double>> const& a){
	return tba::Zebra_team{
		tba::Team_key{"frc1"},//obviously this is ficticous.
		to_optionals(firsts(a)),
		to_optionals(seconds(a))
	};
}

void graph(
	std::vector<double> times,
	std::vector<tba::Zebra_team> const& data,
	std::optional<Display_style> const& display_style,
	std::optional<Period> const& period,
	std::string const& name
){
	vector<vector<Datapoint>> dp;

	for(auto line:data){
		vector<Datapoint> line_out;
		for(auto [t,x,y]:zip(times,line.xs,line.ys)){
			if(x){
				assert(y);
				line_out|=make_pair(t,make_pair(*x,*y));
			}
		}
		dp.push_back(line_out);
	}
	
	for(auto period_now:periods(period)){
		auto f=mapf(
			[=](auto x){
				return filter(
					[=](auto p){ return in_period(p.first,period_now); },
					x
				);
			},
			dp
		);

		for(auto style:styles(display_style)){
			switch(style){
				case Display_style::HEAT:{
					//void heatmap(std::function<double(int,int)> density,std::string name){
					map<pair<int,int>,int> m;
					for(auto elem:flatten(f)){
						m[elem.second]++;
					}
					heatmap([&](int x,int y){ return m[make_pair(x,y)]; },name+"_"+as_string(period_now));
					break;
				}
				case Display_style::LINE:
					plot(
						//to_zebra_team(seconds(f)),
						mapf([](auto x){ return to_zebra_team(seconds(x)); },f),
						name+"_"+as_string(period_now)+"_"+as_string(style)
					);
					break;
				default:
					assert(0);
			}
		}
	}
}

void team_plain(tba::Cached_fetcher &cf,Team team,Args args){
	static const auto YEAR=tba::Year{2020};
	auto d=mapf(
		[&](auto k){
			return zebra_motionworks(cf,k);
		},
		team_matches_year_keys(cf,to_team_key(team),YEAR)
	);
	auto f=filter([](auto x){ return x; },d);
	if(f.empty()){
		cout<<"No data for team:"<<team<<"\n";
		return;
	}
	graph(
		f[0]->times,
		mapf(
			[&](auto x)->tba::Zebra_team{
				return find_team(team,*x);
			},
			f
		),
		args.style,
		args.period,
		as_string(team)
	);
}

void show_points(std::vector<Point> const& a){
	string plotter_name="plotter_pt.py";
	{
		ofstream f(plotter_name.c_str());
		f<<"import numpy as np\n";
		f<<"import matplotlib.pyplot as plt\n";
		//f<<"#a:"<<a<<"\n";
		/*for(auto a:data){
			f<<"xs=[]\n";
			auto python_literal=[](optional<double> x)->string{
				if(!x){
					return "None";
				}
				return as_string(x);
			};
			for(auto elem:a.xs){
				f<<"xs.append("<<python_literal(elem)<<")\n";
			}
			f<<"ys=[]\n";
			for(auto elem:a.ys){
				f<<"ys.append("<<python_literal(elem)<<")\n";
			}
			f<<"plt.plot(xs,ys)\n";
		}*/
		for(auto [x,y]:a){
			f<<"plt.plot(["<<x<<"],["<<y<<"])\n";
		}

		f<<"plt.savefig(\"points.png\")\n";
	}

	//pid_t p=fork();
	//if(p==0){
		auto r=system( ("python "+plotter_name).c_str() );
		assert(r==0);
	//	exit(0);
	//}
}

std::map<Team,bool> defense(std::map<bool,std::map<Team,Point>> a){
	//first, consider defense that might be played by the red team
	//second, consider defense that might be played by the blue team
	
	for(auto defender:a[0]){
		PRINT(defender);
		for(auto opponent:a[1]){
			PRINT(opponent);
			vector<Point> points{
				defender.second,
				opponent.second
			};
			show_points(points);
			nyi
		}
	}
	nyi
}

map<Team,Time> recognize_defense(tba::Cached_fetcher &f,tba::Match_key match){
	//first thing to do: amount of defense played
	//second thing to do: how effective was the defense?
	//third thing to do: how good is a robot depending on whether or not it was defended?

	//own output port
	//on the wall: x=2, y=4

	//opposing output port
	//a little bit out: x=41, y=22

	//opponents' goal
	//x: 5-11
	//y: 13+	

	//own goal:x=52,y=7
	
	//getting through the middle, if the defended is a tall bot (and not endgame, etc.)
	
	//using Timepoint=tuple<std::vector<std::vector<Point>>,std::vector<std::vector<Point>>>;
	//first, just analyze individual positioning
	//worry about how people are moving later
	
	auto z=zebra_motionworks(f,match);
	if(!z) return {};

	std::vector<map<bool,map<Team,Point>>> points;

	for(auto [i,t]:enumerate(z->times)){
		(void)t;
		map<bool,map<Team,Point>> locations;
		auto get_points=[i](std::vector<tba::Zebra_team> const& v){
			std::map<Team,Point> r;
			for(auto a:v){
				assert(i<a.xs.size());
				assert(i<a.ys.size());
				auto x=a.xs[i];
				auto y=a.ys[i];
				if(x){
					assert(y);
					r[to_team(a.team_key)]=make_pair(*x,*y);
				}
			}
			return r;
		};
		locations[0]=get_points(z->alliances.red);
		locations[1]=get_points(z->alliances.blue);
		//PRINT(locations);
		points|=locations;
	}

	mapf(defense,points);
	nyi
}

void recognize_defense(tba::Cached_fetcher &f){
	mapf(
		[&](auto x){ return recognize_defense(f,tba::Match_key{x.key}); },
		event_matches(f,tba::Event_key{"2020orore"})
	);
}

template<typename K,typename V>
void print_n(map<K,V> const& a){
	for(auto [k,v]:a){
		cout<<k<<" ("<<v.size()<<")\n";
		cout<<"\t"<<v<<"\n";
	}
}

void show_events(tba::Cached_fetcher &f){
	auto teams=district_teams_keys(f,tba::District_key{"2020pnw"});
	PRINT(teams)

	map<set<tba::Event_key>,vector<Team>> r;
	for(auto team:teams){
		auto e=team_events_year_keys(f,team,YEAR);
		auto s=to_set(e);
		s-=tba::Event_key{"2020waahs"};
		r[s]|=to_team(team);
	}
	print_n(r);
	//print_r(r);

	map<int,vector<Team>> num;
	for(auto [k,v]:r){
		num[k.size()]|=v;
	}
	//print_r(num);
	print_n(num);
}

int demo(tba::Cached_fetcher &cf){
	//recognize_defense(cf);
	//find_tall_bots(cf);
	show_events(cf);

	return 0;
}

int main1(int argc,char **argv){
	auto args=parse_args(argc,argv);
	std::ifstream f("../tba/auth_key");
	std::string tba_key;
	getline(f,tba_key);
	tba::Cached_fetcher cf{tba::Fetcher{tba::Nonempty_string{tba_key}},tba::Cache{}};
	static const auto YEAR=tba::Year{2020};
	//indenpendent: heat period
	//team match event
	//
	if(args.demo){
		return demo(cf);
	}
	if(args.heat_diff){
		study_deviation(cf);
		return 0;
	}

	if(args.team){
		if(args.match){
			//show how that team did in that match
			auto z=zebra_motionworks(cf,*args.match);
			if(!z){
				cout<<"Data not found.\n";
				return 1;
			}
			graph(
				z->times,
				{find_team(*args.team,*z)},
				args.style,
				args.period,
				as_string(args.team)+"_"+as_string(args.match)
			);
			return 0;
		}else if(args.event){
			//how that team did at that event
			auto tm=team_event_matches(cf,to_team_key(*args.team),*args.event);
			auto k=mapf(
				[&](auto x){
					return zebra_motionworks(cf,tba::Match_key{x.key});
				},
				tm
			);
			auto f=filter([](auto x){ return x; },k);
			assert(f.size());
			graph(
				f[0]->times,
				mapf(
					[&](auto x)->tba::Zebra_team{
						return find_team(*args.team,*x);
					},
					f
				),
				args.style,
				args.period,
				as_string(args.team)+"_"+as_string(args.event)
			);
			return 0;
		}
		team_plain(cf,*args.team,args);
		return 0;
	}
	if(args.match){
		//all teams in this match
		//at some point, will want to add labels for which line is which
		auto z=zebra_motionworks(cf,*args.match);
		if(!z){
			cout<<"Could not find data for that match.\n";
			exit(1);
		}
		graph(z->times,all_teams(*z),args.style,args.period,as_string(args.match));
		return 0;
	}else if(args.event){
		//all teams scheduled to be at this event
		auto teams=event_teams_keys(cf,*args.event);
		for(auto team:teams){
			team_plain(cf,to_team(team),args);
		}
		return 0;
	}
	help();

	nyi
	/*
	 * styles of displays:
	 * heat/line
	 * how many lines/robots to show at once
	 * the option of single display vs many: always do both?
	 * */

	auto e=event_teams_keys(cf,tba::Event_key{"2020orwil"});
	find_tall_bots(cf);

	for(auto t:e){
		std::vector<tba::Zebra_team> paths;
		for(auto match_key:team_matches_year_keys(cf,t,YEAR)){
			auto data=zebra_motionworks(cf,match_key);
			if(!data) continue;
			paths|=find_team(to_team(t),*data);
		}
		using Mode=int;//0=auto,1=tele main, 2=endgame
		map<Mode,map<pair<int,int>,unsigned>> m;
		for(auto elem:paths){
			for(auto [i,point]:enumerate(zip(elem.xs,elem.ys))){
				if(point.first){
					auto mode=[=](){
						if(i<15*10) return 0;
						if(i<(15+135-30)*10) return 1;
						return 2;
					}();
					m[mode][make_pair(*point.first,*point.second)]++;
				}
			}
		}
		for(auto [mode,info]:m){
			heatmap(
				[&](int x,int y){ return info[make_pair(x,y)]; },
				as_string(t)+"_"+as_string(mode)
			);
		}
	}

	assert(args.team);
	auto matches=team_matches_year_keys(cf,to_team_key(*args.team),YEAR);
	//matches={tba::Match_key{"2020orore_f1m1"}};

	vector<tba::Zebra_team> paths;
	for(auto match_key:matches){
		auto data=zebra_motionworks(cf,match_key);
		if(!data) continue;
		/*auto find_path=[&](){
			for(auto x:data->alliances.red){
				if(x.team_key==args.team){
					return x;
				}
			}
			for(auto x:data->alliances.blue){
				if(x.team_key==args.team){
					return flip_field(x);
				}
			}
			assert(0);
		};*/
		auto p=find_team(*args.team,*data);
		plot(p,match_key.get());
		paths|=p;

		largest_jump(data->times,p,match_key.get());
		//cout<<largest_jump(data->times,p)<<"\n";

		//TODO: Make it so that can recognize trench bots
		//Measure top speeds for different robots
		//Look at where the starting locations are to see where like to line up
		//Put in heat charts for auto vs tele
		//Make charts show auto lines w/ markers for start & end
		//Histogram of how fast robots are instantaneously moving
	}

	plot(paths);
	PRINT(limits(paths));

	vector<pair<int,int>> combined;
	for(auto zt:paths){
		for(auto [x,y]:zip(zt.xs,zt.ys)){
			if(x){
				combined|=make_pair(int(*x),int(*y));
			}
		}
	}
	map<pair<int,int>,unsigned> by_loc;
	for(auto elem:combined){
		by_loc[elem]++;
	}
	heatmap(
		[&](int x,int y)->double{
			return by_loc[make_pair(x,y)]; //flipping here because of the direction the plot goes
		},
		"overall"
	);
	return 0;
}

int main(int argc,char **argv){
	try{
		return main1(argc,argv);
	}catch(tba::Decode_error const& a){
		cout<<"Caught: "<<a<<"\n";
		return 1;
	}catch(std::string const& s){
		cout<<s<<"\n";
		return 1;
	}
}
