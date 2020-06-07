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

#ifndef FLAT_MAP_H
#define FLAT_MAP_H

//This is designed purely for speed advantages over std::map or std::unordered_map.
template<typename K,size_t MAX,typename V>
class Flat_map{
	std::array<V,MAX> data;
	std::bitset<MAX> present;
	public:
	Flat_map(){
		present=0;
	}

	Flat_map(std::map<K,V> const& a){
		for(auto [k,v]:a){
			present[int(k)]=1;
			data[int(k)]=v;
		}
	}

	//using iterator=std::pair<K,V>*;
	struct const_iterator{
		const Flat_map *parent;
		size_t i;

		bool operator!=(const_iterator a){
			return !(*this==a);
		}

		bool operator==(const_iterator a){
			return i==a.i;
		}

		std::optional<std::pair<K,V>> operator->(){
			assert(parent);
			assert(parent->present[i]);
			return std::make_pair(K(i),parent->data[i]);
		}

		std::pair<K,V> operator*(){
			assert(parent);
			assert(parent->present[i]);
			return std::make_pair(K(i),parent->data[i]);
		}

		const_iterator& operator++(){
			assert(parent);
			assert(i<MAX);
			i++;
			while(i<MAX && !parent->present[i]){
				i++;
			}
			return *this;
		}
	};

	const_iterator find(K k)const{
		size_t i=size_t(k);
		if(present[i]){
			return const_iterator{this,i};
		}
		return end();
	}

	const_iterator begin()const{
		size_t i;
		for(i=0;i<MAX && !present[i];i++) ;
		return const_iterator{this,i};
	}

	const_iterator end()const{
		return const_iterator{this,MAX};
	}

	bool operator<(Flat_map const& a)const{
		if(present<a.present){
			return 1;
		}
		if(a.present<present){
			return 0;
		}
		/*for(auto [e,ae]:zip(*this,a)){
			if(e<ae) return 1;
			if(ae<e) return 0;
		}*/
		for(auto i:range(MAX)){
			if(present[i]){
				if(data[i]<a.data[i]){
					return 1;
				}
				if(data[i]>a.data[i]){
					return 0;
				}
			}
		}
		return 0;
	}

	Flat_map& operator+=(Flat_map const& a){
		for(auto [k,v]:a){
			size_t i=size_t(k);
			if(present[i]){
				data[i]+=v;
			}else{
				data[i]=v;
				present[i]=1;
			}
		}
		return *this;
	}

	Flat_map& operator/=(float f){
		for(auto i:range(MAX)){
			if(present[i]){
				data[i]/=f;
			}
		}
		return *this;
	}

	V& operator[](K k){
		size_t i=size_t(k);
		if(!present[i]){
			data[i]=V{};
			present[i]=1;
		}
		return data[i];
	}
};

template<typename K,size_t MAX,typename V>
std::ostream& operator<<(std::ostream& o,Flat_map<K,MAX,V> const& a){
	o<<"{ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

#endif
