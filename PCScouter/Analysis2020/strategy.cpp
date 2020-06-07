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

#include<functional>
#include<cmath>
#include "util.h"
#include "int_limited.h"
#include "dist.h"
#include "input_data.h"
#include "capabilities.h"
#include "game.h"

/*To do list:
 * 1) different strategies about who is climbing effects time to do other things
 * 2) Improve quality of random robot match data
 * 3) Make the climb being balanced do something
 * 4) Check for consistency of the thing being balanced on input data
 * */

//start generic code

using namespace std;

//start program-specific code

auto wheel_spins(vector<Input_row> const& a){
	return sum(mapf([](auto x){ return x.wheel_spin; },a));
}

unsigned climb_assists_given(vector<Input_row> const& a){
	return sum(mapf([](auto x){ return x.climb_assists; },a));
}

unsigned climb_assists_rx(vector<Input_row> const& a){
	return sum(mapf([](auto x){ return x.climb_was_assisted; },a));
}

std::vector<Input_row> rand(std::vector<Input_row> const*){
	std::vector<Input_row> r;
	for(auto match:range(1,80)){
		std::vector<Input_row> match_data;
		for(auto alliance:alliances()){
			auto v=mapf(
				[=](auto _){
					(void)_;
					auto r=rand((Input_row*)0);
					r.match=Match(match);
					r.alliance=alliance;
					return r;
				},
				range(3)
			);
			while(wheel_spins(v)>1){
				v[rand()%3].wheel_spin=0;
			}

			while(climb_assists_given(v)>climb_assists_rx(v)){
				auto& x=v[rand()%3].climb_assists;
				if(x) x--;
			}

			while(climb_assists_rx(v)>climb_assists_given(v)){
				v[rand()%3].climb_was_assisted=0;
			}

			match_data|=v;
		}

		//make sure all three robots on the alliance are different
		while(teams(match_data).size()<6){
			match_data[rand()%6].team=rand((Team*)0);
		}

		r|=match_data;
	}
	return r;
}

void write_file(std::string const& path,std::vector<Input_row> const& data){
	ofstream o(path);
	vector<string> cols;
	#define X(A,B) cols|=""#B;
	INPUT_DATA(X)
	#undef X
	o<<join(",",cols)<<"\n";
	for(auto row:data){
		vector<string> items;
		#define X(A,B) items|=as_string(row.B);
		INPUT_DATA(X)
		#undef X
		o<<join(",",items)<<"\n";
	}
}

void run(Team team,std::optional<string> const& path){
	auto data=[=](){
		if(path) return read_csv(*path);

		//use random data if no path specified
		auto v=rand((std::vector<Input_row>*)0);
		write_file("example.csv",v);
		return read_csv("example.csv");
	}();
	sanity_check(data);

	auto rc=robot_capabilities(data);
	show(rc);

	//check that the target team is in the data.
	auto t=teams(data);
	if(!t.count(team)){
		cout<<"Warning: Did not find picking team ("<<team<<") in the data.  Adding zeros.\n";
		rc[team]={};
	}

	auto list=make_picklist(team,rc);
	show_picklist(team,list);
}

int main1(int argc,char **argv){
	Team team{1425};
	auto set_team=[&](vector<string>& a)->int{
		assert(a.size());
		team=Team{stoi(a[0])};
		a=skip(1,a);
		return 0;
	};
	std::optional<std::string> path;
	auto set_path=[&](vector<string>& a)->int{
		assert(a.size());
		path=a[0];
		a=skip(1,a);
		return 0;
	};
	vector<tuple<
		const char *, //name
		const char *, //arg info
		const char *, //msg
		std::function<int(vector<string>&)>
	>> options{
		make_tuple(
			"--team","<TEAM NUMBER>","Create picklist for the given team number",
			std::function<int(vector<string>&)>(set_team)
		),
		make_tuple(
			"--file","<PATH>","Input data file",
			std::function<int(vector<string>&)>(set_path)
		)
	};
	
	auto help=[&](vector<string>&)->int{
		cout<<"Available arguments:\n";
		for(auto [arg,arg_info,msg,func]:options){
			(void)func;
			cout<<"\t"<<arg<<" "<<arg_info<<"\n";
			cout<<"\t\t"<<msg<<"\n";
		}
		exit(0);
	};
	options|=make_tuple(
		"--help","","Display this message",
		std::function<int(vector<string>&)>(help)
	);

	auto arg_list=args(argc,argv);
	arg_list=skip(1,arg_list); //skip name of the binary.

	while(arg_list.size()){
		bool found=0;
		for (auto [arg, arg_info, msg, func] : options) {
			(void)arg_info;
			(void)msg;
			if (arg_list[0] == arg) {
				found = 1;
				arg_list = skip(1, arg_list);
				auto r = func(arg_list);
				if (r) {
					cout << "Failed.\n";
					return r;
				}
			}

			if (found)
				break;
		}
		if(!found){
			cerr<<"Unrecognized arg:"<<arg_list;
			return 1;
		}
	}

	run(team,path);
	return 0;
}

int main(int argc,char **argv){
	try{
		return main1(argc,argv);
	}catch(std::string const& a){
		cout<<"Caught:"<<a<<"\n";
	}
}
