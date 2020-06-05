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
