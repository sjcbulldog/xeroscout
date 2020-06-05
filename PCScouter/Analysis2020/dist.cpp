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

