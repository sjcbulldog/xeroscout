#include "preview.h"

#if 0
#include<iomanip>
#include "util.h"
#include "input_data.h"
#include "game.h"
#include "capabilities.h"

using namespace std;

//start generic code

template<typename T>
auto tail(T t){ return cdr(t); }

std::vector<std::string> operator+(std::vector<std::string> const& a,std::vector<const char *> const& b){
	auto r=a;
	for(auto elem:b){
		r|=elem;
	}
	return r;
}

std::vector<std::string> operator+(std::vector<const char *> const& a,std::vector<std::string> const& b){
	std::vector<std::string> r;
	for(auto elem:a) r|=elem;
	for(auto elem:b) r|=elem;
	return r;
}

#define CTOR(A) [](auto x){ return (A)(x); }

template<typename T,size_t N>
std::vector<T> to_vec(std::array<T,N> const& a){
	std::vector<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

template<typename T,size_t N>
string join(std::array<T,N> const& a){
	return join(to_vec(a));
}

template<typename T,size_t N>
auto mean(std::array<T,N> const& a){
	return mean(to_vec(a));
}

//start program-specific code

string preview(double d){
	stringstream ss;
	ss<<setprecision(2);
	ss<<d;
	return ss.str();
}

string preview(Dist d){ return preview(mean(d)); }

Alliance_capabilities to_alliance(std::vector<Robot_capabilities> const& a){
	Alliance_capabilities alliance{};
	assert(a.size()<=3);
	for(auto [i,x]:enumerate(a)){
		alliance[i]=x;
	}
	return alliance;
}

auto expected_score(std::vector<Robot_capabilities> const& a){
	return expected_score(to_alliance(a));
}

//using Match_outcome=std::pair<double,double>;//bonus RP, points scored
//How to estimate how large the distribution of opponent scores is ...
//compare pair of poisson distributions w/ lambda=expected # of points
//might have to do some digging to find the old Skellam cdf function in
//terms of exp, etc.

/*std::vector<std::pair<Match_outcome,Alliance_strategy>> top_strategies(Alliance_capabilities const& cap){
	auto s=reversed(sorted(mapf(
		[=](auto strat){
			return expected_score(cap,strat);
		},
		alliance_climb_strategies()
	)));
	return take(5,s);
}*/

Alliance_strategy best_strategy(Alliance_capabilities const& cap){
	return argmax(
		[=](auto strat){ return expected_score(cap,strat); },
		alliance_climb_strategies()
	);
}

enum class Analysis_mode{
	QUAL,PLAYOFF
};

std::ostream& operator<<(std::ostream& o,Analysis_mode a){
	switch(a){
		case Analysis_mode::QUAL:
			return o<<"QUAL";
		case Analysis_mode::PLAYOFF:
			return o<<"PLAYOFF";
		default:
			assert(0);
	}
}

void make_preview(std::vector<Team> const& teams,std::map<Team,Robot_capabilities> rc){
	auto red=take(3,teams);
	auto blue=skip(3,teams);
	//show matchup for just this match
	auto reds=to_alliance(mapf([&](auto team){ return rc[team]; },red));
	auto blues=to_alliance(mapf([&](auto team){ return rc[team]; },blue));
	
	ofstream f("preview.html");
	f<<"<html>";
	f<<"<head>";
	f<<title(" Match preview: "+join(", ",MAP(as_string,teams)));
	f<<"</head>";
	f<<"<body>";
	f<<"<table border>";
	f<<tr(tag("th colspan=4 bgcolor=red","Red")+tag("th rowspan=2","Match Preview")+tag("th colspan=4 bgcolor=blue","Blue"));
	f<<"<tr>";
	f<<join(mapf([](auto x){ return tag("th bgcolor=red",x); },MAP(as_string,red)+vector{"Mean"}));
	f<<join(mapf([](auto x){ return tag("th bgcolor=blue",x); },vector{"Mean"}+MAP(as_string,blue)));
	f<<"</tr>";
	#define X(A,B) f<<tr(\
		join(mapf([](auto x){ return td(preview(x.B)); },reds))+\
		td(preview(mean(reds).B))+\
		th(""#B)+\
		td(preview(mean(blues).B))+\
		join(mapf([](auto x){ return td(preview(x.B)); },blues))\
	);
	ROBOT_CAPABILITIES(X)
	#undef X
	f<<"<tr>";
	f<<join(mapf([](auto x){ return td(expected_score(vector{x})); },reds));
	f<<td(expected_score(reds));
	f<<th("Score estimate");
	f<<td(expected_score(blues));
	f<<join(mapf([](auto x){ return td(expected_score(vector{x})); },blues));
	f<<"</tr>";
	f<<"<tr>";
	auto red_strat=best_strategy(reds);
	f<<join(MAP(td,red_strat));
	f<<td("")+th("Optimal Climb Strategy");
	f<<td("")+join(MAP(td,best_strategy(blues)));
	f<<"</tr>";
	f<<"</table>";
	f<<"</body>";
	f<<"</html>";
	//totals for each of the alliances
	//and suggested strategies for each of them
	//
	//in match strategy suggestions:
	//1) how do you want to climb?
	//2) do you want the ball scoring to go just for points or do you want to aim for # of balls scored?
	//3) is it worth cutting a climb in order to do the second bonus?
}
#endif

int main(int argc,char **argv){
	auto a=tail(args(argc,argv));
	std::optional<Analysis_mode> analysis_mode;

	if(a[0]=="--qual"){
		analysis_mode=Analysis_mode::QUAL;
	}
	if(a[0]=="--playoff"){
		analysis_mode=Analysis_mode::PLAYOFF;
	}
	if(!analysis_mode){
		analysis_mode=Analysis_mode::PLAYOFF;
	}

	if(a.size()!=7){
		cout<<"Match preview:\n";
		cout<<"Required arguments:\n";
		vector<string> v{
			"Input filename (csv)"
		};
		for(auto a:options((Alliance_station*)0)){
			v|="Team number located at "+as_string(a);
		}
		for(auto [i,elem]:enumerate_from(1,v)){
			cout<<"\t"<<i<<": "<<elem<<"\n";
		}
	}
	auto path=a[0];
	auto teams=mapf([](auto s){ return Team(stoi(s)); },tail(a));

	//parse the input data
	auto data=read_csv(path);
	sanity_check(data);

	//run it through the capabilities
	auto rc=robot_capabilities(data);
	make_preview(teams,rc);
}
