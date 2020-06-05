#include<iostream>
#include<array>
#include "util.h"

using namespace std;

using Location=std::pair<size_t,size_t>;

template<typename T,size_t N,size_t M>
class Array2d{
	std::array<std::array<T,M>,N> data;

	public:
	T const& operator()(unsigned x,unsigned y)const{
		assert(x<N);
		assert(y<M);
		return data[x][y];
	}

	T& operator[](Location a){
		assert(a.first<N);
		assert(a.second<M);
		return data[a.first][a.second];
	}
};

template<typename T,size_t N,size_t M>
std::vector<Location> locations(Array2d<T,N,M> const&){
	return cross(range(N),range(M));
}

#define STRATEGIES(X)\
	X(CLIMB)\
	X(BALLONLY)\
	X(ROTATION)\
	X(POSITION)\
	X(INVALID)

enum class Strategy{
#define X(A) A,
	STRATEGIES(X)
#undef X
};

std::ostream& operator<<(std::ostream& o,Strategy a){
	#define X(A) if(a==Strategy::A) return o<<""#A;
	STRATEGIES(X)
	#undef X
	assert(0);
}

static const int WIDTH=150;
static const int HEIGHT=50;
using Result=Array2d<Strategy,WIDTH,HEIGHT>;

string color(Strategy strat){
	return tag("td bgcolor="+[=]()->string{
		switch(strat){
			case Strategy::CLIMB: return "red";
			case Strategy::BALLONLY: return "yellow";
			case Strategy::ROTATION: return "blue";
			case Strategy::POSITION: return "green";
			case Strategy::INVALID: return "white";
			default: assert(0);
		}
	}(),"");
}

string key(){
	stringstream ss;
	ss<<h2("Key");
	ss<<"<table>";
#define X(A) ss<<tr(td(Strategy::A)+color(Strategy::A));
	STRATEGIES(X)
#undef X
	ss<<"</table>";
	return ss.str();
}

std::vector<std::pair<char,bool>> mark_end(std::string const& s){
	std::vector<std::pair<char,bool>> r;
	for(auto at=begin(s);at!=end(s);++at){
		r|=make_pair(*at,at+1==end(s));
	}
	return r;
}

string join(string const& a,string const& b){
	stringstream ss;
	for(auto [c,end]:mark_end(b)){
		ss<<c;
		if(!end) ss<<a;
	}
	return ss.str();
}

void write(string const& path,Result const& a){
	auto heading="1425 2020 In-match strategy guide";
	ofstream f(path.c_str());
	f<<"<html>";
	f<<head(title(heading));
	f<<key();
	f<<"<body>";
	f<<"<table>";
	f<<"<tr>";
	f<<tag("td colspan=2 rowspan=2","");
	f<<tag("th colspan="+as_string(WIDTH),"Time left");
	f<<"</tr>";
	auto show_time=[](auto i){
		if(i==149) return th(join("<br>","Auto"));
		if(i==135) return th(join("<br>","Tele"));
		if(i%5==0){ return th(join("<br>",as_string(i))); }
		return td("");
	};
	f<<tr(join(mapf(show_time,reversed(range(WIDTH)))));
	for(auto [i,balls_scored]:enumerate(reversed(range(HEIGHT)))){
		f<<"<tr>";
		if(i==0){
			f<<tag("th rowspan="+as_string(HEIGHT),"Balls scored");
		}
		f<<th(as_string(balls_scored));
		for(auto time_left:reversed(range(WIDTH))){
			f<<color(a(time_left,balls_scored));
		}
		f<<"</tr>";
	}
	f<<"</table>";
	f<<"</body>";
	f<<"</html>";
}

Strategy get_recommendation(Location const& a){
	auto t=a.first;
	auto balls=a.second;
	if(t<20) return Strategy::CLIMB;
	static const auto bps_max=49.0/(135-20);
	static const auto bps_rotation=29.0/(135-20);
	auto time_elapsed=150-t;
	if(time_elapsed==0) return Strategy::POSITION;
	const auto bps=(0.0+balls)/time_elapsed;
	if(bps>=bps_max){
		return Strategy::POSITION;
	}
	if(bps>bps_rotation){
		return Strategy::ROTATION;
	}
	return Strategy::BALLONLY;
}

Strategy stage3_recommendation(Location a){
	auto [time_left,balls_scored]=a;
	if(balls_scored<29) return Strategy::INVALID;
	(void)time_left;
	return Strategy::BALLONLY;
}

int main1(){
	Result a{};
	for(auto location:locations(a)){
		a[location]=get_recommendation(location);
	}
	write("handheld.html",a);
	for(auto location:locations(a)){
		a[location]=stage3_recommendation(location);
	}
	write("stage3.html",a);
	return 0;
}

using Time=pair<bool,int>; //is auto, time left in period

string as_time(Time t){
	stringstream ss;
	if(t.first){
		ss<<"Auto";
	}else{
		ss<<"Tele";
	}
	ss<<" "<<t.second;
	return ss.str();
}

vector<Time> times(){
	mapf(
		[](auto x){ return "Auto "+as_string(x); },
		reversed(range(0,16,5))
	)+
	mapf(
		[](auto x){ return "Tele "+as_string(x); },
		reversed(range(0,136,5))
	);
	nyi
}

string stage1(){
	stringstream ss;
	ss<<h2("Stage 1");
	ss<<"<table border>";
	ss<<"<tr>";
	ss<<tag("td colspan=2 rowspan=2","");
	ss<<tag("th colspan=15","Time");
	ss<<tr(mapf(
		[](auto x){ return tag("th valign=top",join("<br>",x)); },
		mapf(
			[](auto x){ return "Auto "+as_string(x); },
			reversed(range(0,16,5))
		)+
		mapf(
			[](auto x){ return "Tele "+as_string(x); },
			reversed(range(0,136,5))
		)
	));
	ss<<"</tr>";
	for(auto [i,x]:enumerate(reversed(range(0,15,5)))){
		ss<<"<tr>";
		if(i==0){
			ss<<tag("th rowspan=3","Balls scored");
		}
		ss<<th(x);
		for(auto x:range(20)){
			ss<<td(x);
		}
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

int main(){
	write_file("stage1.html",stage1());
}
