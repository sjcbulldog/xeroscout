#include "opr.h"
#include<cassert>
#include<iostream>
#include<map>
#include<vector>
#include<set>
#include<cmath>
#include "qr_solve.hpp"
#include "util.h"
#include "input_data.h"
#include "../tba/tba.h"
#include "../tba/db.h"
#include "../tba/data.h"

using namespace std;

template<typename K,typename V>
std::vector<std::pair<V,K>> swap_pairs(std::map<K,V> const& a){
	return swap_pairs(to_vec(a));
}

Team to_team(tba::Team_key a){
	return Team{atoi(a.str().c_str()+3)};
}

template<typename A,typename B,typename C>
std::vector<A> firsts(std::vector<std::tuple<A,B,C>> const& a){
	return mapf([](auto x){ return get<0>(x); },a);
}

template<typename A,typename B,typename C>
std::vector<B> seconds(std::vector<std::tuple<A,B,C>> const& a){
	return mapf([](auto x){ return get<1>(x); },a);
}

template<typename T>
std::vector<T>& operator|=(std::vector<T>& a,T t){
	a.push_back(t);
	return a;
}

template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> const& a){
	std::vector<T> r;
	for(auto elem:a){
		for(auto x:elem){
			r|=x;
		}
	}
	return r;
}

template<typename T>
bool contains(vector<T> const& a,T t){
	for(auto elem:a){
		if(t==elem){
			return 1;
		}
	}
	return 0;
}

/*template<typename T>
std::ostream& operator<<(std::ostream& o,std::vector<T> const& a){
	o<<"[ ";
	for(auto x:a){
		o<<x<<" ";
	}
	return o<<"]";
}*/

std::vector<std::pair<std::vector<Team>,int>> results(){
	std::vector<std::pair<std::vector<Team>,int>> r;
	for(int i=0;i<10;i++){
		r|=std::make_pair(
			std::vector<Team>{
				rand((Team*)0),
				rand((Team*)0),
				rand((Team*)0)
			},
			rand()%200
		);
	}
	return r;
}

std::vector<int> range(int n){
	std::vector<int> r;
	for(int i=0;i<n;i++){
		r|=i;
	}
	return r;
}

template<typename A,typename B>
std::vector<std::pair<A,B>> zip1(std::vector<A> const& a,B const* b){
	std::vector<std::pair<A,B>> r;
	for(auto [i,a1]:enumerate(a)){
		r|=std::make_pair(a1,b[i]);
	}
	return r;
}

std::vector<std::tuple<std::vector<Team>,std::vector<Team>,int>> demo_data(){
	auto alliance=[](){
		return vector{
			rand((Team*)0),
			rand((Team*)0),
			rand((Team*)0)
		};
	};
	return mapf(
		[=](auto i){
			(void)i;
			return make_tuple(
				alliance(),
				alliance(),
				rand()%200
			);
		},
		range(10)
	);
}

std::map<Team,double> solve(
	std::vector<std::tuple<std::vector<Team>,std::vector<Team>,int>> const& data
){
	auto teams=to_vec(to_set(flatten(firsts(data))|flatten(seconds(data))));
	map<Team,size_t> team_index=to_map(swap_pairs(enumerate(teams)));
	vector<double> a;//the matrix
	vector<double> b;
	for(auto [red,blue,red_margin]:data){
		for(auto team:teams){
			a|=[&](){
				if(contains(red,team)){
					return 1;
				}
				if(contains(blue,team)){
					return -1;
				}
				return 0;
			}();
		}
		b|=double(red_margin);
	}
	double *x2=svd_solve(teams.size(),data.size(),&(a[0]),&(b[0]));
	std::map<Team,double> r;
	for(auto [i,value]:zip1(range(teams.size()),x2)){
		r[teams[i]]=value;
	}
	return r;
}

std::map<Team,double> solve(std::vector<std::pair<std::vector<Team>,int>> const& x){
	std::map<Team,int> rename;
	auto teams=to_vec(to_set(flatten(firsts(x))));
	for(auto [i,team]:enumerate(teams)){
		rename[team]=i;
	}
	vector<double> a;
	vector<double> b;
	for(auto [teams1,points]:x){
		for(auto team:teams){
			if(contains(teams1,team)){
				a|=1.0;
			}else{
				a|=0.0;
			}
		}
		b|=double(points);
	}
	/*cout<<"a:\n";
	cout<<a.size()<<"\n";
	//cout<<a<<"\n";
	for(auto i:range(x.size())){
		for(auto j:range(teams.size())){
			auto n=i*x.size()+j;
			cout<<a[n]<<" ";
		}
		cout<<"\n";
	}
	cout<<"b:\n";
	cout<<b.size()<<"\n";
	cout<<b<<"\n";*/
	double *x2=svd_solve(teams.size(),x.size(),&(a[0]),&(b[0]));
	/*for(size_t i=0;i<teams.size();i++){
		cout<<i<<":"<<x2[i]<<"\n";
	}*/
	std::map<Team,double> r;
	for(auto [i,value]:zip1(range(teams.size()),x2)){
		r[teams[i]]=value;
	}
	return r;
}

enum class Detail_item{
	#define X(A,B) B,
	TBA_MATCH_SCORE_BREAKDOWN_2020_ALLIANCE(X)
	#undef X
};

std::ostream& operator<<(std::ostream& o,Detail_item a){
	#define X(A,B) if(a==Detail_item::B) return o<<""#B;
	TBA_MATCH_SCORE_BREAKDOWN_2020_ALLIANCE(X)
	#undef X
	assert(0);
}

template<typename A,typename B,typename C>
std::ostream& operator<<(std::ostream& o,std::tuple<A,B,C> const& a){
	o<<"("<<get<0>(a)<<","<<get<1>(a)<<","<<get<2>(a)<<")";
	return o;
}

double as_double(int i){
	return i;
}

double as_double(tba::Init_line a){
	return a==tba::Init_line::Exited;
}

double as_double(tba::Endgame a){
	if(a==tba::Endgame::None) return 0;
	if(a==tba::Endgame::Park) return 5;
	if(a==tba::Endgame::Hang) return 25;
	PRINT(a);
	nyi
}

double as_double(tba::Target_color){
	return 0;
}

double as_double(tba::Rung_level a){
	if(a==tba::Rung_level::IsLevel) return 1;
	if(a==tba::Rung_level::NotLevel) return 0;
	PRINT(a);
	nyi
}

std::vector<Team> to_teams(std::vector<tba::Team_key> const& a){
	return mapf(to_team,a);
}

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::tuple<A,B> const& a){
	return o<<"("<<get<0>(a)<<","<<get<1>(a)<<")";
}

template<typename A,typename B>
std::vector<std::pair<A,B>> to_pairs(std::vector<std::tuple<A,B>> const& a){
	return mapf(
		[](auto x){
			return make_pair(get<0>(x),get<1>(x));
		},
		a
	);
}

double square(double x){
	return x*x;
}

double stddev(std::vector<double> const& a){
	auto m=mean(a);
	return sqrt(mean(mapf(
		[=](auto x){
			return square(x-m);
		},
		a
	)));
}

bool analyze_item(
	std::pair<
		Detail_item,
		std::vector<std::tuple<std::vector<Team>,int>>
	> const& p
){
	auto [name,data]=p;
	PRINT(name);
	//PRINT(data);
	auto s=solve(to_pairs(data));
	//PRINT(s);
	auto v=values(s);
	auto mu=mean(v);
	auto sigma=stddev(v);
	PRINT(mu);
	PRINT(sigma);
	auto z_score=[=](double a){
		return (a-mu)/sigma;
	};
	auto zs=map_values(z_score,s);
	//print_r(zs);
	print_lines(sorted(swap_pairs(s)));
	return 0;
}

void f(){
	//for an event, go calculate the outliers of OPR performance
	//get list of (team,which item,z score)
	
	std::ifstream f("../tba/auth_key");
	std::string tba_key;
	getline(f,tba_key);
	tba::Cached_fetcher cf{tba::Fetcher{tba::Nonempty_string{tba_key}},tba::Cache{}};

	tba::Event_key event_key{"2020orore"};
	const tba::Year YEAR{2020};

	using T=tuple<std::vector<Team>,int>;

	map<Detail_item,vector<T>> m;
	auto add=[&](
		std::vector<tba::Team_key> const& teams,
		tba::Match_Score_Breakdown_2020_Alliance const& x
	){
		#define X(A,B) m[Detail_item::B]|=std::make_pair(to_teams(teams),as_double(x.B));
		TBA_MATCH_SCORE_BREAKDOWN_2020_ALLIANCE(X)
		#undef X
	};
	for(auto x:event_matches(cf,event_key)){
		if(!x.score_breakdown) continue;
		//PRINT(x);
		tba::Match_Score_Breakdown_2020 x1=get<tba::Match_Score_Breakdown_2020>(*x.score_breakdown);
		add(x.alliances.red.team_keys,x1.red);
		add(x.alliances.blue.team_keys,x1.blue);
	}

	mapf(analyze_item,m);
}

int main(){
	f();
	{
		auto x=results();
		auto s=solve(x);
		//PRINT(s);
		print_lines(sorted(swap_pairs(to_vec(s))));
	}
	{
		auto d=demo_data();
		auto s=solve(d);
		print_lines(sorted(swap_pairs(to_vec(s))));
	}
	return 0;

	/*
	 *
	 * going to try to solve this first:
	 *  (a)       (b)
	 * [0 1]     [ 4]
	 * [2 0] x = [20]
	 *
	 * should result in 10,4
	 * */
	int m=2;
	int n=2;
	double a[]={0,20,2,0};
	double b[]={8,20};

	double *x2=svd_solve(m,n,a,b);
	assert(x2);

	for(int i=0;i<n;i++){
		cout<<i<<":"<<x2[i]<<"\n";
	}

	//delete[]a;
	//delete[]b;
	delete[]x2;
}
