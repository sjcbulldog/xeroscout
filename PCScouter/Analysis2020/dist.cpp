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

#include "dist.h"

#include<map>
#include "util.h"

using Px=double;//probability; should be 0-1.

Dist::Dist(){
	(*this)[0]=1;
}

Dist mean(std::vector<Dist> const& a){
	//just put in each of the items with a reduced probability.
	Dist r;
	if(a.size()){
		r.clear();
		for(auto d:a){
			for(auto [v,p]:d){
				r[v]+=p/a.size();
			}
		}
	}
	return r;
}

Dist operator+(Dist const& a,Dist const& b){
	//PRINT(sum(values(a)));
	//PRINT(sum(values(b)));

	Dist r;
	r.clear();
	for(auto [av,ap]:a){
		for(auto [bv,bp]:b){
			r[av+bv]+=ap*bp;
		}
	}
	//PRINT(sum(values(r)));
	//PRINT(r);
	return r;
}

Dist& operator+=(Dist& a,Dist const& b){
	return a=(a+b);
}

Dist to_dist(std::vector<unsigned> const& a){
	Dist r;
	if(a.empty()){
		return r;
	}

	r.erase(0);
	Px per_count=1.0/a.size();
	for(auto elem:a){
		r[elem]+=per_count;
	}
	return r;
}

Dist min(unsigned a,Dist b){
	Dist r;
	for(auto [val,p]:b){
		r[std::min(a,val)]+=p;
	}
	return r;
}

Px operator>=(Dist const& a,unsigned u){
	auto f=filter(
		[u](auto p){ return p.first>=u; },
		to_vec(a)
	);
	return sum(seconds(f));
}

double mean(Dist const& a){
	double total=sum(MAP(product,a));
	auto weight=sum(values(a));//this should usually be 1.
	return total/weight;
}

