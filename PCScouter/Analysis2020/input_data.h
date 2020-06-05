#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include "int_limited.h"

#define SHOW(A,B) o<<""#B<<":"<<a.B<<" ";

using Team=Int_limited<1,1000*10>;

Team rand(Team const*);

using Match=Int_limited<1,200>; //qual match number

enum class Alliance{RED,BLUE};

std::ostream& operator<<(std::ostream&,Alliance);
Alliance parse(Alliance const*,std::string const&);
std::array<Alliance,2> options(Alliance const*);
Alliance rand(Alliance const*);
std::vector<Alliance> alliances();

using Climb_assists=Int_limited<0,2>;
using Balls=Int_limited<0,100>;

#define INPUT_DATA(X)\
	X(Team,team)\
	X(Match,match)\
	X(Alliance,alliance)\
	X(bool,auto_line)\
	X(Balls,auto_low)\
	X(Balls,auto_high)\
	X(Balls,tele_low)\
	X(Balls,tele_high)\
	X(bool,wheel_spin)\
	X(bool,wheel_color)\
	X(bool,climbed)\
	X(Climb_assists,climb_assists)\
	X(bool,climb_was_assisted)\
	X(bool,park)\
	X(bool,balanced)

struct Input_row{
	INPUT_DATA(INST)
};

bool operator<(Input_row const&,Input_row const&);
std::ostream& operator<<(std::ostream&,Input_row const&);
Input_row rand(Input_row const*);

std::set<Team> teams(std::vector<Input_row> const&);
std::vector<Input_row> read_csv(std::string const& path);

static const int TURN_THRESHOLD=29;
static const int COLOR_PICK_THRESHOLD=49;

unsigned balls_towards_shield(std::vector<Input_row> const&);

void sanity_check(std::vector<Input_row> const&);

#endif
