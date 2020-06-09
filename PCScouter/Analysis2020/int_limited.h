//
// Copyright(C) 2020 by  Eric Rippey
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
