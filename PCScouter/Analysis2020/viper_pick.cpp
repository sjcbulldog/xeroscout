#include "viper.h"

#include<iostream>
#include<fstream>
#include<string.h>
#include "input_data.h"
#include "capabilities.h"
#include "game.h"

using namespace std;

struct Args{
	string path;
	Team team;
};

void help(){
	cout<<"Arguments:\n";
	cout<<"--path\n";
	cout<<"\tThe path to the data file.\n";
	cout<<"--team\n";
	cout<<"\tTeam number to do the picking\n";
	cout<<"--help\n";
	cout<<"\tShow this message\n";
	exit(0);
}

Args parse_args(int argc,char **argv){
	(void)argc;
	Args r;
	r.team=Team{1425};
	for(auto at=argv+1;*at;++at){
		if(string(*at)=="--help"){
			help();
		}else if(string(*at)=="--team"){
			at++;
			assert(*at);
			r.team=Team(stoi(*at));
		}else if(string(*at)=="--path"){
			at++;
			assert(*at);
			r.path=*at;
		}else{
			cout<<"Invalid argument.\n";
			help();
		}
	}
	if(r.path==""){
		cout<<"Must specify input file\n";
		exit(1);
	}
	return r;
}

int main(int argc,char **argv){
	auto args=parse_args(argc,argv);
	auto rc=parse_viper(args.path);
	show(rc);
	auto list=make_picklist(args.team,rc);
	show_picklist(args.team,list);
	return 0;
}
