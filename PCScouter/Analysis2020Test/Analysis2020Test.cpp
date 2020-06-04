// Analysis2020Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "InputData.h"
#include "Analysis2020.h"

int main(int ac, char **av)
{
	std::string error;

	ac--;
	av++;

	xero::analysis2020::InputData indata;
	xero::analysis2020::Analysis2020 analysis(indata);

	std::string datafile = *av++;
	if (!indata.readCSV(datafile, error))
	{
		std::cerr << "error: could not read data file '" << datafile << "'";
		std::cerr << "      " << error;
		return 1;
	}

	analysis.run();
}
