#include "CsvReader.h"
#include "TeamPicker2020.h"
#include "PickListFormatter.h"
#include <iostream>
#include <string>
#include <chrono>

void usage()
{
	std::cout << "usage: [--noisy] picklist2020 TEAM datafile ouputfile" << std::endl;
	std::cout << "       TEAM is the number of the team picking" << std::endl;
}

int main(int ac, char **av)
{
	int team;
	bool noisy = false;

	ac--;
	av++;

	std::string arg = *av;
	if (arg == "--noisy")
	{
		ac--;
		av++;
		noisy = true;
	}

	if (ac != 3)
	{
		usage();
		return 1;
	}

	size_t pos;
	bool parsed = false;
	arg = *av++;

	try {
		team = std::stoi(arg, &pos);
		if (pos == arg.length())
			parsed = true;
	}
	catch (const std::invalid_argument&)
	{
	}

	if (!parsed)
	{
		std::cout << "picklist2020: invalid TEAM argument '" << arg << "', expected an integer team number" << std::endl;
		usage();
		return 1;
	}

	auto start = std::chrono::high_resolution_clock::now();

	arg = *av++;
	CsvReader reader(true);
	if (!reader.readFile(std::filesystem::path(arg), CsvReader::HeaderType::Headers))
	{
		usage();
		return 1;
	}

	TeamPicker2020 picker(reader.headers(), reader.data());
	picker.setNoisy(noisy);
	if (!picker.createPickList(team))
	{
		std::cerr << "picklist2020: pick list creation failed" << std::endl;
		return 1;
	}

	if (!PickListFormatter::outputData(team, *av, picker.picks(), PickListFormatter::OutputType::CSV))
	{
		std::cerr << "picklist2020: cannout write output file" << std::endl;
		return 1;
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	std::cout << "========================================" << "\n";
	std::cout << "Duration " << dur.count() << " millisecond" << "\n";
	std::cout << "========================================" << "\n";

	return 0;
}

