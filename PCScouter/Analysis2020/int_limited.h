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

#ifndef INT_LIMITED_H
#define INT_LIMITED_H

#include<cassert>
#include<iosfwd>
#include "util.h"

template<int MIN,int MAX>
class Int_limited{
	int i;

	public:
	Int_limited():i(MIN){}

	explicit Int_limited(int value):i(value){
		assert(value>=MIN && value<=MAX);
	}

	operator int()const{ return i; }

	Int_limited operator--(int){
		auto r=*this;
		i--;
		assert(i>=MIN);
		return r;
	}
};

template<int MIN,int MAX>
Int_limited<MIN,MAX> parse(Int_limited<MIN,MAX> const*,std::string const& s){
	return Int_limited<MIN,MAX>(stoi(s));
}

template<int MIN,int MAX>
std::vector<Int_limited<MIN,MAX>> options(Int_limited<MIN,MAX> const*){
	return mapf(
		[](auto x){ return Int_limited<MIN,MAX>(x); },
		range(MIN,MAX+1)
	);
}

template<int MIN,int MAX>
Int_limited<MIN,MAX> rand(Int_limited<MIN,MAX> const*){
	return Int_limited<MIN,MAX>(MIN+rand()%(MAX-MIN+1));
}

template<int MIN,int MAX>
int sum(std::vector<Int_limited<MIN,MAX>> const& a){
	int r=0;
	for(auto elem:a) r+=elem;
	return r;
}

#endif
