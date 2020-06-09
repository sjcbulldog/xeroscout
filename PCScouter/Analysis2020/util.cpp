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

#include "util.h"
#include<cmath>

std::vector<int> range(unsigned lim){
	std::vector<int> r(lim);
	std::iota(begin(r),end(r),0);
	return r;
}

std::vector<int> range(int start,int lim){
	assert(lim>=start);
	std::vector<int> r(lim-start);
	std::iota(begin(r),end(r),start);
	return r;
}

void write_file(std::string filename,std::string data){
	std::ofstream f(filename);
	assert(f.good());
	f<<data;
}

int min(int a,unsigned b){
	assert(b<INT_MAX);
	return std::min(a,(int)b);
}

char as_hex_digit(int i){
	assert(i>=0);
	assert(i<16);
	if(i<=9) return '0'+i;
	return 'a'+(i-10);
}

std::string as_hex(int i){
	assert(i>=0);
	assert(i<=255);
	return std::string()+as_hex_digit(i>>4)+as_hex_digit(i&0xf);
}

int rerange(int min_a,int max_a,int min_b,int max_b,int value){
	auto f=(value-min_a)/(max_a-min_a+0.0);
	auto out=min_b+f*(max_b-min_b);
	return out;
}

int sum(std::pair<int,int> p){
	return p.first+p.second;
}

std::string first_word(std::string const& s){
	auto f=s.find(' ');
	if(f==std::string::npos) return s;
	return s.substr(0,f);
}

std::string join(std::vector<std::string> const& a){
	std::stringstream ss;
	for(auto const& elem:a) ss<<elem;
	return ss.str();
}

std::string join(std::string const& delim,std::vector<std::string> const& a){
	std::stringstream ss;
	auto e=end(a);
	for(auto at=begin(a);at!=e;at++){
		ss<<*at;
		if(at+1!=e){
			ss<<delim;
		}
	}
	return ss.str();
}

std::vector<bool> bools(){
	std::vector<bool> r;
	r|=0;
	r|=1;
	return r;
}

std::vector<std::string> split(std::string const& s,char target){
	std::vector<std::string> r;
	std::stringstream ss;
	for(auto c:s){
		if(c==target){
			r|=ss.str();
			ss.str("");
		}else{
			ss<<c;
		}
	}
	if(ss.str().size()){
		r|=ss.str();
	}
	return r;
}

int atoi(std::string const& s){
	return atoi(s.c_str());
}

std::vector<std::string> args(int argc,char **argv){
	std::vector<std::string> r;
	for(int i=0;i<argc;i++){
		r|=argv[i];
	}
	return r;
}

size_t sum(std::multiset<bool> const& m){
	return FILTER(id,m).size();
}

float mean(std::multiset<bool> const& m){
	assert(m.size());
	return (0.0+sum(m))/m.size();
}

size_t sum(std::vector<bool> const& a){
	size_t r=0;
	for(auto elem:a){
		if(elem) r++;
	}
	return r;
}

float mean(std::vector<bool> const& a){
	assert(a.size());
	return (0.0+sum(a))/a.size();
}

std::string pop(std::vector<std::string>& v){
	//warning! this is O(n) and modifies its argument
	assert(v.size());
	auto r=v[0];
	v.erase(v.begin());
	//PRINT(v);
	return r;
}

void indent_by(unsigned int i){
	for(auto _:range(i)){
		(void)_;
		std::cout<<"\t";
	}
}

int rand(int const*){ return rand(); }
bool rand(bool const*){ return rand()%2; }
unsigned rand(unsigned const*){ return rand(); }

double mean(double a,double b){
	return (a+b)/2;
}

int parse(int const*,std::string const& s){
	return stoi(s);
}

std::string take(size_t n,std::string const& s){
	return s.substr(0,n);
}

bool parse(bool const*,std::string const& s){
	if(s=="0") return 0;
	if(s=="1") return 1;
	throw "Unparsed bool: \""+s+"\"";
}

double product(std::vector<double> const& a){
	double r=1;
	for(auto elem:a) r*=elem;
	return r;
}

double geomean(std::vector<double> const& a){
	assert(a.size());
	return pow(product(a),1.0/a.size());
}

double mean(std::multiset<unsigned> a){ return mean(to_vec(a)); }
