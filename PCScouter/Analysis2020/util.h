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
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#ifndef UTIL_H
#define UTIL_H

#include<vector>
#include<set>
#include<array>
#include<iostream>
#include<iterator>
#include<numeric>
#include<cassert>
#include<fstream>
#include<climits>
#include<map>
#include<algorithm>
#include<sstream>
#include<bitset>
#include<variant>
#include <optional>

#define nyi { std::cout<<"nyi "<<__FILE__<<":"<<__LINE__<<"\n"; exit(44); }
#define PRINT(X) { std::cout<<""#X<<":"<<(X)<<"\n"; }

size_t sum(std::multiset<bool> const&);
float mean(std::multiset<bool> const&);
float mean(std::vector<bool> const&);
std::string pop(std::vector<std::string>&);

template<typename T,typename T2>
std::vector<T>& operator|=(std::vector<T> &a,T2 t){
	a.push_back(std::move(t));
	return a;
}

template<typename T>
std::vector<T>& operator|=(std::vector<T> &a,std::vector<T> const& b){
	a.insert(a.end(),b.begin(),b.end());
	return a;
}

template<typename T>
std::set<T>& operator|=(std::set<T>& a,std::vector<T> const& b){
	a.insert(b.begin(),b.end());
	return a;
}

template<typename T>
std::set<T>& operator|=(std::set<T>& a,T t){
	a.insert(t);
	return a;
}

template<typename T>
std::vector<T> to_vec(std::set<T> a){
	return std::vector<T>{begin(a),end(a)};
}

template<typename T>
std::set<T> operator|(std::set<T> a,std::set<T> const& b){
	a.insert(begin(b),end(b));
	return a;
}

template<typename T>
bool operator&(T t,std::set<T> const& s){
	return s.count(t)!=0;
}

template<typename T,size_t LEN>
std::ostream& operator<<(std::ostream& o,std::array<T,LEN> const& a){
	o<<"[ ";
	std::ostream_iterator<T> out_it(o," ");
	std::copy(begin(a),end(a),out_it);
	return o<<"]";
}

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::pair<A,B> const& a){
	return o<<"("<<a.first<<","<<a.second<<")";
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::vector<T> const& a){
	o<<"[ ";
	/*
	Not sure why this commented part doesn't work sometimes.
	std::ostream_iterator<T> out_it(o," ");
	std::copy(begin(a),end(a),out_it);*/
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::set<T> const& a){
	o<<"{ ";
	std::ostream_iterator<T> out_it(o," ");
	std::copy(begin(a),end(a),out_it);
	return o<<"}";
}

template<typename T>
std::vector<T> range(T lim){
	std::vector<T> r(lim);
	std::iota(begin(r),end(r),0);
	return r;
}

template<size_t LEN>
std::array<size_t,LEN> range_st(){
	std::array<size_t,LEN> r;
	std::iota(begin(r),end(r),0);
	return r;
}

template<typename T>
std::vector<T> range(T start,T lim){
	assert(lim>=start);
	std::vector<T> r(lim-start);
	std::iota(begin(r),end(r),start);
	return r;
}


template<typename T,typename STEP>
std::vector<T> range(T start,T lim,STEP step){
	assert(lim>=start);
	std::vector<T> r;
	for(auto i=start;i<lim;i+=step){
		r|=i;
	}
	return r;
}

void write_file(std::string filename,std::string data);
int min(int a,unsigned b);
char as_hex_digit(int);
std::string as_hex(int);
int rerange(int min_a,int max_a,int min_b,int max_b,int value);
int sum(std::pair<int,int>);
size_t sum(std::vector<bool> const&);

template<typename T,size_t LEN>
T sum(std::array<T,LEN> const& a){
	return std::accumulate(begin(a),end(a),T{});
}

template<typename T>
T sum(std::vector<T> v){
	return std::accumulate(begin(v),end(v),T{});
}

template<typename T>
T max(std::vector<T> const& a){
	assert(a.size());
	return *std::max_element(begin(a),end(a));
}

template<typename T>
T mean(std::vector<T> v){
	assert(v.size());
	//The cast is here to avoid making the sum get converted to unsigned.
	return sum(v)/(int)v.size();
}

template<typename A,typename B>
std::vector<std::pair<A,B>> cross(std::vector<A> const& a,std::vector<B> const& b){
	std::vector<std::pair<A,B>> r;
	for(auto const& a1:a){
		for(auto const& b1:b){
			r|=std::make_pair(a1,b1);
		}
	}
	return r;
}

template<typename Func,typename T>
auto group(Func f,T v)->std::map<decltype(f(*begin(v))),T>{
	std::map<decltype(f(*begin(v))),T> r;
	for(auto elem:v){
		r[f(elem)]|=elem;
	}
	return r;
}

template<typename Func,typename T>
T filter(Func f,T const& t){
	T r;
	std::copy_if(begin(t),end(t),back_inserter(r),f);
	return r;
}

template<typename Func,typename T,size_t LEN>
std::vector<T> filter(Func f,std::array<T,LEN> const& t){
	std::vector<T> r;
	std::copy_if(begin(t),end(t),back_inserter(r),f);
	return r;
}

template<typename Func,typename T>
std::set<T> filter(Func f,std::set<T> const& t){
	std::set<T> r;
	std::copy_if(begin(t),end(t),inserter(r,r.end()),f);
	return r;
}

template<typename Func,typename T>
std::multiset<T> filter(Func f,std::multiset<T> const& t){
	std::multiset<T> r;
	std::copy_if(begin(t),end(t),inserter(r,r.end()),f);
	return r;
}

template<typename T>
T id(T t){ return t; }

#define FILTER(F,X) filter([&](auto x){ return F(x); },X)

template<typename Func,typename T>
auto mapf(Func f,std::vector<T> const& v) -> std::vector< decltype(f(v[0])) > {
	std::vector<decltype(f(v[0]))> r;
	r.reserve(v.size());
	std::transform(begin(v),end(v),std::back_inserter(r),f);
	return r;
}

template<typename Func,typename K,typename V>
auto mapf(Func f,std::map<K,V> const& v) -> std::vector< decltype(f(*begin(v))) > {
	std::vector<decltype(f(*begin(v)))> r(v.size());
	std::transform(begin(v),end(v),begin(r),f);
	return r;
}

template<typename Func,typename T,size_t LEN>
auto mapf(Func f,std::array<T,LEN> const& v) -> std::array< decltype(f(v[0])) , LEN> {
	std::array<decltype(f(v[0])),LEN> r;
	std::transform(begin(v),end(v),begin(r),f);
	return r;
}

#define MAP(F,V) mapf([&](auto const& elem){ return F(elem); },V)

template<typename T>
class Nonempty_vector{
	std::vector<T> data;
};

template<typename T>
std::string as_string(T const& t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

std::string first_word(std::string const&);

template<typename T>
std::string tag(std::string const& t,T const& body){
	std::stringstream ss;
	ss<<"<"<<t<<">";
	ss<<body;
	ss<<"</"<<first_word(t)<<">";
	return ss.str();
}

template<typename T>
std::string html(T a){ return tag("html",a); }

template<typename T>
std::string table(T a){ return tag("table",a); }

template<typename T>
std::string tr(T a){ return tag("tr",a); }

template<typename T>
std::string td(T a){ return tag("td",a); }

template<typename T>
std::string th(T a){ return tag("th",a); }

template<typename T>
std::string head(T a){ return tag("head",a); }

template<typename T>
auto body(T a){ return tag("body",a); }

template<typename T>
auto title(T a){ return tag("title",a); }

template<typename T>
auto h1(T a){ return tag("h1",a); }

template<typename T>
auto h2(T a){ return tag("h2",a); }

template<typename T>
auto small(T a){ return tag("small",a); }

std::string join(std::vector<std::string> const&);
std::string join(std::string const&,std::vector<std::string> const&);
std::vector<bool> bools();

#define RM_REF(X) typename std::remove_reference<X>::type

template<typename T>
void print_lines(T const& a){
	/*std::ostream_iterator<RM_REF(decltype(*begin(a)))> out_it(std::cout,"\n");
	std::copy(begin(a),end(a),out_it);*/
	for(auto const& elem:a){
		std::cout<<elem<<"\n";
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::optional<T> const& a){
	if(a) return o<<*a;
	return o<<"NULL";
}

template<typename K,typename V>
std::map<K,V> without_key(std::map<K,V> a,K key){
	a.erase(a.find(key));
	return a;
}

template<typename T>
std::vector<T> sorted(std::vector<T> a){
	std::sort(begin(a),end(a));
	return a;
}

template<typename T>
std::vector<T> reversed(std::vector<T> a){
	std::reverse(begin(a),end(a));
	return a;
}

template<typename T>
std::vector<std::pair<size_t,T>> enumerate_from(size_t i,std::vector<T> const& a){
	std::vector<std::pair<size_t,T>> r;
	for(auto elem:a){
		r|=make_pair(i++,elem);
	}
	return r;
}

template<typename Func,typename T>
T take_first(Func f,std::multiset<T> m){
	for(auto elem:m){
		if(f(elem)){
			return elem;
		}
	}
	assert(0);
}

#define RM_CONST(X) typename std::remove_const<X>::type

#define ELEM(X) RM_CONST(RM_REF(decltype(*std::begin(X))))

template<typename Func,typename T>
auto mapf(Func f,std::multiset<T> const& a)->
	std::vector<decltype(f(*begin(a)))>
{
	std::vector<decltype(f(*begin(a)))> r;
	for(auto elem:a){
		r|=f(elem);
	}
	return r;
}

template<typename Func,typename T>
auto mapf(Func f,std::set<T> const& a)->
	std::vector<decltype(f(*begin(a)))>
{
	std::vector<decltype(f(*begin(a)))> r;
	for(auto elem:a){
		r|=f(elem);
	}
	return r;
}

template<typename T>
T mode(std::vector<T> v){
	assert(v.size());
	auto m=std::multiset<T>(begin(v),end(v));
	auto max_count=max(mapf([&](auto x){ return m.count(x); },m));
	return take_first([&](auto x){ return m.count(x)==max_count; },m);
}

template<typename T>
std::vector<T> non_null(std::vector<std::optional<T>> a){
	std::vector<T> r;
	for(auto elem:a){
		if(elem){
			r|=*elem;
		}
	}
	return r;
}

template<typename T>
std::multiset<T>& operator|=(std::multiset<T>& a,T t){
	a.insert(t);
	return a;
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::multiset<T> const& a){
	o<<"{ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename K,typename V>
std::ostream& operator<<(std::ostream& o,std::map<K,V> const& a){
	o<<"{ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename T>
std::multiset<T>& operator|=(std::multiset<T>& a,std::multiset<T> b){
	a.insert(b.begin(),b.end());
	return a;
}

template<typename T>
std::multiset<T> operator+(std::multiset<T> a,std::multiset<T> b){
	a|=b;
	return a;
}

template<typename A,typename B>
std::vector<std::pair<A,B>> zip(std::vector<A> const& a,std::vector<B> const& b){
	return mapf(
		[&](auto i){ return std::make_pair(a[i],b[i]); },
		range(min(a.size(),b.size()))
	);
}

template<typename A,typename B,size_t LEN>
std::array<std::pair<A,B>,LEN> zip(std::array<A,LEN> const& a,std::array<B,LEN> const& b){
	std::array<std::pair<A,B>,LEN> r;
	std::transform(
		begin(a),end(a),begin(b),begin(r),
		[](auto a1,auto b1){ return std::make_pair(a1,b1); }
	);
	return r;
}

template<typename A,typename B,size_t LEN>
std::vector<std::pair<A,B>> zip(std::vector<A> const& a,std::array<B,LEN> const& b){
	std::vector<std::pair<A,B>> r;
	std::transform(
		begin(a),begin(a)+std::min(a.size(),LEN),begin(b),back_inserter(r),
		[](auto a1,auto b1){ return std::make_pair(a1,b1); }
	);
	return r;
}

template<typename A,typename B>
auto zip(A const& a,B const& b){
	std::vector<std::pair<ELEM(a),ELEM(b)>> r;
	auto ai=std::begin(a);
	auto bi=std::begin(b);
	for(;ai!=a.end() && bi!=b.end();++ai,++bi){
		r|=std::make_pair(*ai,*bi);
	}
	return r;
}

template<typename Func,typename K,typename V>
auto map_values(Func f,std::map<K,V> const& in){
	std::map<K,decltype(f(in.begin()->second))> r;
	for(auto [k,v]:in){
		r[k]=f(v);
	}
	return r;
}

#define MAP_VALUES(f,v) map_values([&](auto x){ return f(x); },v)

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& a){
	return mapf([](auto x){ return x.second; },a);
}

template<typename T>
std::multiset<T> flatten(std::vector<std::multiset<T>> a){
	std::multiset<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

void indent_by(unsigned indent);

template<typename T>
void print_r(unsigned indent,T const& t){
	indent_by(indent);
	std::cout<<t<<"\n";
}

template<typename A,typename B>
void print_r(unsigned indent,std::pair<A,B> const& p){
	indent_by(indent);
	std::cout<<"pair\n";
	print_r(indent+1,p.first);
	print_r(indent+1,p.second);
}

template<typename K,typename V>
void print_r(unsigned indent,std::map<K,V> const& a){
	indent_by(indent);
	std::cout<<"map\n";
	for(auto elem:a){
		print_r(indent+1,elem);
	}
}

template<typename T>
void print_r(unsigned indent,std::vector<T> const& a){
	indent_by(indent);
	std::cout<<"vector\n";
	for(auto elem:a){
		print_r(indent+1,elem);
	}
}

template<typename T>
void print_r(T t){
	print_r(0,t);
}

template<typename T>
std::set<T> to_set(std::vector<T> const& v){
	return std::set<T>(begin(v),end(v));
}

template<typename T,size_t LEN>
std::array<T,LEN> sorted(std::array<T,LEN> a){
	std::sort(begin(a),end(a));
	return a;
}

template<typename T>
std::set<T> choose(size_t num,std::set<T> a){
	if(num==0){
		return {};
	}
	assert(a.size());
	auto other=choose(num-1,a);
	auto left=filter([other](auto x){ return other.count(x)==0; },a);
	return other|std::set<T>{to_vec(left)[rand()%left.size()]};
}

template<typename T1,typename T2>
std::vector<T1>& operator|=(std::vector<T1>& a,std::vector<T2> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::set<T>& operator-=(std::set<T> &a,T const& t){
	auto it=a.find(t);
	if(it!=a.end()){
		a.erase(it);
	}
	return a;
}

template<typename T,typename T2>
std::set<T>& operator-=(std::set<T> &a,T2 b){
	auto it=a.find(T{b});
	if(it!=a.end()){
		a.erase(it);
	}
	return a;
}

template<typename T>
std::set<T>& operator-=(std::set<T> &a,std::set<T> const& b){
	for(auto elem:b){
		a-=elem;
	}
	return a;
}

std::vector<std::string> split(std::string const& s,char target);

template<typename T>
std::vector<std::tuple<T,T,T>> cross3(std::array<std::vector<T>,3> in){
	std::vector<std::tuple<T,T,T>> r;
	for(auto a:in[0]){
		for(auto b:in[1]){
			for(auto c:in[2]){
				r|=make_tuple(a,b,c);
			}
		}
	}
	return r;
}

template<typename A,typename B>
std::pair<A,B> operator+(std::pair<A,B> a,std::pair<A,B> b){
	return std::make_pair(a.first+b.first,a.second+b.second);
}

template<typename T>
T sum(std::tuple<T,T,T> t){
	return std::get<0>(t)+std::get<1>(t)+std::get<2>(t);
}

template<typename Func,typename K,typename V>
std::map<K,V> filter(Func f,std::map<K,V> a){
	std::map<K,V> r;
	for(auto p:a){
		if(f(p)){
			r[p.first]=p.second;
		}
	}
	return r;
}

template<typename T>
std::vector<T> take(size_t lim,std::vector<T> in){
	if(in.size()<=lim) return in;
	return std::vector<T>(begin(in),begin(in)+lim);
}

template<typename A,typename T>
auto mean_else(A a,T t)->decltype(mean(a)){
	if(a.empty()) return t;
	return mean(a);
}

template<typename T>
std::vector<T> skip(size_t i,std::vector<T> v){
	//note: this is implemented in a very slow way.
	for(auto _:range(i)){
		(void)_;
		if(v.size()){
			v.erase(v.begin());
		}
	}
	return v;
}

template<typename A,typename B>
std::vector<B> seconds(std::vector<std::pair<A,B>> a){
	return mapf([](auto x){ return x.second; },a);
}

int atoi(std::string const&);
std::vector<std::string> args(int argc,char **argv);

template<typename K,typename V>
std::vector<std::pair<K,V>> to_vec(std::map<K,V> a){
	std::vector<std::pair<K,V>> r;
	for(auto p:a){
		r|=p;
	}
	return r;
}

#define INST(A,B) A B;

//kind of nutty that this isn't built in.
template<size_t N>
bool operator<(std::bitset<N> a,std::bitset<N> b){
	return a.to_ullong()<b.to_ullong();
}

int rand(int const*);
bool rand(bool const*);
unsigned rand(unsigned const*);

template<typename A,typename B>
std::vector<std::pair<std::optional<A>,std::optional<B>>> zip_extend(std::vector<A> const& a,std::vector<B> const& b){
	auto a_at=begin(a);
	auto b_at=begin(b);
	auto a_end=end(a);
	auto b_end=end(b);
	std::vector<std::pair<std::optional<A>,std::optional<B>>> r;
	while(a_at!=a_end || b_at!=b_end){
		r|=make_pair(
			[=]()->std::optional<A>{
				if(a_at==a_end) return std::nullopt;
				return *a_at;
			}(),
			[=]()->std::optional<B>{
				if(b_at==b_end) return std::nullopt;
				return *b_at;
			}()
		);
		if(a_at!=a_end) a_at++;
		if(b_at!=b_end) b_at++;
	}
	return r;
}

template<typename T>
std::vector<std::pair<size_t,T>> enumerate(std::vector<T> const& a){
	std::vector<std::pair<size_t,T>> r;
	size_t i=0;
	for(auto const& elem:a){
		r|=std::make_pair(i++,elem);
	}
	return r;
}

int parse(int const*,std::string const&);
bool parse(bool const*,std::string const&);
unsigned parse(unsigned const*,std::string const&);

template<typename A,typename B>
std::vector<A> firsts(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto x){ return x.first; },a);
}

template<typename T>
double mean_d(std::vector<T> v){
	assert(v.size());
	double x=sum(v);
	return x/v.size();
}

template<typename K,typename V>
std::map<K,V> to_map(std::vector<std::pair<K,V>> const& a){
	std::map<K,V> r;
	for(auto [k,v]:a) r[k]=v;
	return r;
}

//int rand(int const*){ return rand(); }
//bool rand(bool const*){ return rand()%2; }
//unsigned rand(unsigned const*){ return rand(); }

template<typename T>
std::multiset<T> to_multiset(std::vector<T> const& a){
	return std::multiset<T>(begin(a),end(a));
}

template<typename T>
T max(std::multiset<T> const& a){
	assert(a.size());
	T r=*begin(a);
	for(auto elem:a){
		r=std::max(r,elem);
	}
	return r;
}

template<typename T,size_t N>
T max(std::array<T,N> const& a){
	static_assert(N);
	T r=a[0];
	for(auto i:range(size_t(1),N)){
		r=std::max(r,a[i]);
	}
	return r;
}

template<typename T>
T min(std::vector<T> const& a){
	assert(a.size());
	T r=*begin(a);
	for(auto elem:a){
		r=std::min(r,elem);
	}
	return r;
}

template<typename T>
auto dist(std::vector<T> const& a){
	auto m=to_multiset(a);
	std::map<T,size_t> r;
	for(auto elem:a){
		r[elem]=m.count(elem);
	}
	return reversed(sorted(mapf(
		[](auto a){
			return std::make_pair(a.second,a.first);
		},
		r
	)));
}

std::string take(size_t,std::string const&);

template<typename Func,typename T>
std::vector<T> sort_by(std::vector<T> a,Func f){
	std::sort(begin(a),end(a),[&](auto x,auto y){ return f(x)<f(y); });
	return a;
}

template<typename T>
std::set<T> to_set(std::multiset<T> const& a){
	return std::set<T>(begin(a),end(a));
}

template<typename T>
std::vector<T> rand(std::vector<T> const*){
	return mapf(
		[](auto _){
			(void)_;
			return rand((T*)0);
		},
		range(rand()%100)
	);
}

template<typename Func,typename K,typename V>
std::map<K,V> filter_keys(Func f,std::map<K,V> a){
	std::map<K,V> r;
	for(auto [k,v]:a){
		if(f(k)){
			r[k]=v;
		}
	}
	return r;
}

template<typename K,typename V>
std::map<K,V> without_key(K const& k,std::map<K,V> a){
	a.erase(k);
	return a;
}

template<typename K,typename V>
std::set<K> keys(std::map<K,V> a){
	return to_set(mapf(
		[](auto p){ return p.first; },
		a
	));
}

double mean(double,double);

template<typename T>
std::vector<T> cdr(std::vector<T> a){
	if(a.size()) a.erase(begin(a));
	return a;
}

double product(std::vector<double> const&);
double geomean(std::vector<double> const&);

template<typename Func,typename T>
T argmax(Func f,std::vector<T> const& a){
	assert(a.size());
	auto x=f(a[0]);
	T r=a[0];
	for(auto elem:cdr(a)){
		auto x1=f(elem);
		if(x1>x){
			x=x1;
			r=elem;
		}
	}
	return r;
}

template<typename T>
std::vector<T> operator+(std::vector<T> a,std::vector<T> b){
	a|=b;
	return a;
}

template<typename T>
std::vector<std::pair<T,bool>> mark_end(std::vector<T> a){
	return mapf(
		[=](auto p){
			auto [i,v]=p;
			return make_pair(v,i==a.size()-1);
		},
		enumerate(a)
	);
}

template<typename T>
T min(T a,T b,T c){ return std::min(a,std::min(b,c)); }

template<typename A,typename B,typename C>
std::vector<std::tuple<A,B,C>> zip(std::vector<A> const& a,std::vector<B> const& b,std::vector<C> const& c){
	return mapf(
		[&](auto i){
			return make_tuple(a[i],b[i],c[i]);
		},
		range(min(a.size(),b.size(),c.size()))
	);
}

template<typename T>
std::vector<T> to_vec(std::multiset<T> a){
	return std::vector<T>(a.begin(),a.end());
}

double mean(std::multiset<unsigned> const&);

//template<typename T>
//auto mean_d(multiset<T> a){ return mean_d(to_vec(a)); }

template<typename... A>
std::ostream& operator<<(std::ostream& o,std::variant<A...> const& a){
        std::visit([&](auto &&elem){ o<<elem; },a);
        return o;
}

template<typename A,typename B>
std::vector<std::pair<B,A>> swap_pairs(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto p){ return std::make_pair(p.second,p.first); },a);
}

template<typename T>
std::vector<T> operator|(std::vector<T> const& a,std::vector<T> const& b){
	std::vector<T> r;
	r|=a;
	r|=b;
	return r;
}

#endif
