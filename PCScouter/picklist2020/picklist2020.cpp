#include "CsvReader.h"
#include "TeamPicker2020.h"
#include "OutputFormatter.h"
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
	std::string pickfile = "picklist";
	std::string capfile = "robot_capabilities";
	std::string infile = "data.csv";

	ac--;
	av++;

	std::string arg = *av;
	if (arg == "--noisy")
	{
		ac--;
		av++;
		noisy = true;
	}

	if (ac < 2)
	{
		usage();
		return 1;
	}

	size_t pos;
	bool parsed = false;
	arg = *av++;
	ac--;

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

	if (ac > 0)
	{
		infile = *av++;
		ac--;
	}

	if (ac > 0)
	{
		pickfile = *av++;
		ac--;
	}

	if (ac > 0)
	{
		capfile = *av++;
		ac--;
	}

	CsvReader reader(true);
	if (!reader.readFile(std::filesystem::path(infile), CsvReader::HeaderType::Headers))
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

	if (!OutputFormatter::outputPicklist(team, pickfile + ".html", picker.picks(), OutputFormatter::OutputType::HTML))
	{
		std::cerr << "picklist2020: cannout write picklist output file" << std::endl;
		return 1;
	}

	if (!OutputFormatter::outputPicklist(team, pickfile + ".csv", picker.picks(), OutputFormatter::OutputType::CSV))
	{
		std::cerr << "picklist2020: cannout write picklist output file" << std::endl;
		return 1;
	}

	std::list<const RobotCapabilities*> robots = picker.robots();
	if (!OutputFormatter::outputRobotCapabilities(capfile + ".html", robots, OutputFormatter::OutputType::HTML))
	{
		std::cerr << "picklist2020: cannout write robot capabilities output file" << std::endl;
		return 1;
	}

	if (!OutputFormatter::outputRobotCapabilities(capfile + ".csv", robots, OutputFormatter::OutputType::CSV))
	{
		std::cerr << "picklist2020: cannout write robot capabilities output file" << std::endl;
		return 1;
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	std::cout << "========================================" << "\n";
	std::cout << "Duration " << dur.count() << " millisecond" << "\n";
	std::cout << "========================================" << "\n";

	return 0;
}

