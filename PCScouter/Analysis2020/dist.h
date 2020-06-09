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

#ifndef DIST_H
#define DIST_H

#include<map>
#include "util.h"

using Px=double;//probability; should be 0-1.

struct Dist:public std::map<unsigned,Px>{
	//models a probability distribution with discrete outcomes.
	Dist();
};

Dist mean(std::vector<Dist> const&);
Dist operator+(Dist const&,Dist const&);
Dist& operator+=(Dist& a,Dist const&);
Dist to_dist(std::vector<unsigned> const&);

template<size_t N>
Dist sum(std::array<Dist,N> const& a){
	Dist r;
	for(auto elem:a) r+=elem;
	return r;
}

Dist min(unsigned a,Dist b);
Px operator>=(Dist const&,unsigned);

template<typename A,typename B>
double product(std::pair<A,B> const& a){
	return a.first*a.second;
}

double mean(Dist const&);

#endif
