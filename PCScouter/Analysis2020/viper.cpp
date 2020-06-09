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

#include "viper.h"
#include<iostream>
#include<fstream>
#include<string.h>
#include "input_data.h"
#include "capabilities.h"
#include "game.h"

using namespace std;

bool contains(string needle,string haystack){
	return !!strstr(haystack.c_str(),needle.c_str());
}

vector<string> split(string s){
	vector<string> r;
	stringstream current;
	for(auto c:s){
		if(isblank(c)){
			if(current.str().size()){
				r|=current.str();
				current.str("");
			}
		}else{
			current<<c;
		}
	}
	if(current.str().size()){
		r|=current.str();
	}
	return r;
}

Dist point_dist(double value){
	Dist r;
	r.clear();
	if(value>=0){
		r[value]=1;
	}else{
		r[0]=1;
	}
	return r;
}

map<Team,Robot_capabilities> parse_viper(string const& path){
	ifstream f(path.c_str());
	string s;
	while(f.good() && !contains("Teleop Outer",s)) getline(f,s);
	
	map<Team,Robot_capabilities> r;
	while(f.good()){
		getline(f,s);
		if(s=="") continue;
		auto x=split(s);
		assert(x.size()==14);
		//Column names:
		//Team    OPR
		//OPRP    Auto Line       Climb   Auto    Auto Inner      Auto Outer      Auto Bottom     Teleop  Teleop Inner    Teleop Outer    Teleop Bottom   Penalty
		auto team=Team{stoi(x[0])};
		auto d=[&](int i){ return stod(x[i]); };
		auto opr=d(1);
		auto oprp=d(2);
		auto auto_line=d(3)/5;
		auto climb=d(4)/25; //going to just assume that it's from hanging and not park, etc.
		auto auto_=d(5);
		auto auto_inner=d(6)/3;
		auto auto_outer=d(7)/2;
		auto auto_bottom=d(8);
		auto teleop=d(9);
		auto teleop_inner=d(10)/3;
		auto teleop_outer=d(11)/2;
		auto teleop_bottom=d(12);
		auto penalty=d(13);

		r[team]=Robot_capabilities{
			auto_line,
			auto_bottom,
			auto_outer,
			auto_inner,
			teleop_bottom,
			teleop_outer,
			teleop_inner,
			point_dist(auto_bottom+auto_outer+auto_inner),
			point_dist(teleop_bottom+teleop_outer+teleop_inner),
			.3, //wheel spin - assume it is done 30% of the time
			.1, //wheel color picking - assume done 30% of the time
			climb,
			0,//assist2
			0,//assist1
			0,//climb was assisted
			0,//park
			0//balance
		};

		(void)opr;
		(void)oprp;
		(void)auto_;
		(void)teleop;
		(void)penalty;
	}
	return r;
}

