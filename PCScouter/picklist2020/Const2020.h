#pragma once
class Const2020
{
public:
	Const2020() = delete;
	~Const2020() = delete;

	//
// The chances that a ball shot into the high goal went into the
// inner port
//
	static constexpr double InnerOdds = 0.2;

	//
	// The total length of teleop
	//
	static constexpr double TeleopLength = 135.0;

	//
	// The total length assumed to be spent climbing
	//
	static constexpr double EndGameLength = 20.0;

	//
	// The number of balls necessary to turn the wheel
	//
	static constexpr int WheelTurnThreshold = 29;

	//
	// The number of balls necessary to turn the wheel to a color
	//
	static constexpr int WheelColorThreshold = 49;

	//
	// Scoring values
	//
	static const constexpr int AutoLinePoints = 5;
	static const constexpr int AutoBallLowPoints = 2;
	static const constexpr int AutoBallOuterPoints = 4;
	static const constexpr int AutoBallInnerPoints = 6;
	static const constexpr int TeleBallLowPoints = 1;
	static const constexpr int TeleBallOuterPoints = 2;
	static const constexpr int TeleBallInnerPoints = 3;
	static const constexpr int WheelSpinPoints = 10;
	static const constexpr int WheelColorPoints = 20;
	static const constexpr int ParkPoints = 5;
	static const constexpr int ClimbPoints = 25;
	static const constexpr int BalancePoints = 15;
};

