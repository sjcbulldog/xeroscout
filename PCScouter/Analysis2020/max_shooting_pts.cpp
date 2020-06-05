#include<iostream>
#include<vector>
#include<boost/math/distributions/normal.hpp>
#include "util.h"

using boost::math::normal;
using namespace std;

/*This is just going to try to do an approximation of where the max # of points is for a shooting target.  Going to start with just throwing a bunch of points up and doing a grid search.  Obviously, this is not optimal in terms of time or accuracy.  
 *
 * angle offset from straight on (degrees),
 * vertical std dev (inches),
 * horizontal std dev (inches)
 * yields:
 * vertical offset (inches)
 * horizontal offset (inches)
 *
 * expected result:
 * -when centered, aim for center, horizontally
 * -when offset a little bit, wanders a bit towards the rear target
 * -when offset gets big enough, goes back to trying for exactly the center because the inner target cannot be hit
 * 
 * for rev1, just worry about the horizontal component, and ignore the vertical component?
 * for rev2, use the vertical component, but assume that the ball has a perfectly flat trajectory
 * for rev3, have some info about ball trajectory going up or down inside the target
 *
 * */

using Inch=double;
using Degrees=double;

static const Inch OUTER_GOAL_WIDTH=30;//inches
static const Inch INNER_GOAL_WIDTH=13;
static const Inch BALL_WIDTH=7;

double expected_outer_points(
	Degrees theta,
	Inch sigma_x,
	Inch sigma_y,
	Inch dx, //offset towards the right
	Inch dy //offset upwards
){
	assert(sigma_x>=0);
	assert(sigma_y>=0);

	//not being used in v1
	(void)sigma_y; 
	(void)dy;

	//1) calculate the width of the outer goal from perspective the shooting from
	double width=30*cos(theta/360*2*M_PI);

	//2) subtract width of the ball
	double available_width=width-BALL_WIDTH;

	//3) see what this looks like as far as # of standard deviations out
	double z1=(-available_width/2-dx)/sigma_x;
	double z2=(available_width/2-dx)/sigma_x;

	//4) consult z table to see what the probabilities are
	normal s;
	double px=cdf(s,z2)-cdf(s,z1);
	return px*2;
}

double deg2rad(Degrees a){ return 2*M_PI/360*a; }

double sin_deg(Degrees a){ return sin(deg2rad(a)); }
double cos_deg(Degrees a){ return cos(deg2rad(a)); }

double expected_inner_points(
	Degrees theta,
	Inch sigma_x,
	Inch sigma_y,
	Inch dx,
	Inch dy
){
	assert(sigma_x>=0);
	assert(sigma_y>=0);

	(void)sigma_y;
	(void)dy;

	static const double TARGET_DEPTH=2*12+5.25;

	//PRINT(theta);
	//PRINT(dx);
	/*
	 *basic layout appearance:
	 * ----
	 *  --
	 *
	 * first, calculate occlusion based on the outer goal
	 * */
	Inch target_shadowed_left=max(0.0,TARGET_DEPTH*sin_deg(theta)-(OUTER_GOAL_WIDTH-INNER_GOAL_WIDTH)/2);
	Inch target_shadowed_right=max(0.0,TARGET_DEPTH*sin_deg(-theta)-(OUTER_GOAL_WIDTH-INNER_GOAL_WIDTH)/2);

	auto target_start_left=-cos_deg(theta)*(INNER_GOAL_WIDTH/2-target_shadowed_left)+BALL_WIDTH/2;
	auto target_start_right=cos_deg(theta)*(INNER_GOAL_WIDTH/2-target_shadowed_right)-BALL_WIDTH/2;

	if(target_start_left>target_start_right){
		//if so, then the target is totally occluded
		return 0;
	}

	auto z1=(target_start_left+dx)/sigma_x;
	auto z2=(target_start_right+dx)/sigma_x;

	normal n;
	auto px=(cdf(n,z2)-cdf(n,z1));
	//PRINT(px);
	//cout<<"\n";
	//cout<<setprecision(2);
	//cout<<"\n"<<theta<<"\t"<<dx<<"\t"<<z1<<"\t"<<z2<<"\t"<<px;
	return 3*px;
}

double expected_points(
	Degrees theta,
	Inch sigma_x,
	Inch sigma_y,
	Inch dx,
	Inch dy
){
	auto px_outer=expected_outer_points(theta,sigma_x,sigma_y,dx,dy)/2;
	auto px_inner=expected_inner_points(theta,sigma_x,sigma_y,dx,dy)/3;
	return (px_outer-px_inner)*2+3*px_inner;
}

std::pair<double,double> best_offset(
	double theta, //degrees
	double sigma_x, //inches
	double sigma_y //inches
){
	return argmax(
		[=](auto a){
			//return expected_outer_points(theta,sigma_x,sigma_y,a.first,a.second);
			//return expected_inner_points(theta,sigma_x,sigma_y,a.first,a.second);
			return expected_points(theta,sigma_x,sigma_y,a.first,a.second);
		},
		mapf(
			[](auto x){ return std::make_pair(x,0); },
			range(-INNER_GOAL_WIDTH/2,INNER_GOAL_WIDTH/2+1,.1)
		)
	);
}

int main(){
	double sigma_x=3;
	double sigma_y=3;
	cout<<setprecision(2)<<"\n";
	for(auto theta:range(0,90,2)){
		auto expected_unadjusted=expected_points(theta,sigma_x,sigma_y,0,0);
		auto adjusted_target=best_offset(theta,sigma_x,sigma_y);
		auto expected_adjusted=expected_points(theta,sigma_x,sigma_y,adjusted_target.first,adjusted_target.second);
		cout<<theta<<"\t"<<adjusted_target<<"\t";
		cout<<expected_adjusted<<"\t";
		cout<<expected_unadjusted<<"\t";
		auto pt_diff=expected_adjusted-expected_unadjusted;
		cout<<pt_diff*50<<"\n";
	}
}
