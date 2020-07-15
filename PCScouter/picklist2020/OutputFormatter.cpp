#include "OutputFormatter.h"
#include "RobotCapabilities.h"
#include <fstream>

bool OutputFormatter::outputRobotCapabilities(const std::string& filename, std::list<const RobotCapabilities*>& robots, OutputType type)
{
	bool ret = false;

	if (type == OutputType::CSV)
		ret = outputRobotCapabilitiesCSV(filename, robots);
	else if (type == OutputType::HTML)
		ret = outputRobotCapabilitiesHTML(filename, robots);

	return ret;
}

bool OutputFormatter::outputRobotCapabilitiesCSV(const std::string& filename, std::list<const RobotCapabilities*>& robots)
{
	//
	// TODO: figure out how to store distributions in CSV files
	//
	return false;
}

bool OutputFormatter::outputRobotCapabilitiesHTML(const std::string& filename, std::list<const RobotCapabilities*>& robots)
{
	if (robots.size() == 0)
		return false;

	std::ofstream out(filename);
	if (!out.is_open())
		return false;

	auto doubleColumns = RobotCapabilities::doubleColumnNames();
	auto distColumns = RobotCapabilities::distColumnNames();

	out << "<!DOCTYPE html>";
	out << "<html lang=\"en\">";
	out << "<head>";
	out << "<meta charset=\"utf-8\"/>";
	out << "<title>Robot Capabilities</title>";
	out << "</head>";
	out << "<h1>Robot Capabilities</h1>";
	out << "<table style=\"border-collapse: collapse;\">";

	out << "<tr>";
	out << "<th style=\"font-weight:bold;border: 2px solid black;\">Team</th>";

	for (const std::string& name : doubleColumns)
	{
		out << "<th style=\"font-weight:bold;border: 2px solid black;\">" << name << "</th>";
	}

	for (const std::string& name : distColumns)
	{
		out << "<th style=\"font-weight:bold;border: 2px solid black;\">" << name << "</th>";
	}

	out << "</tr>";

	for (auto r : robots)
	{
		out << "<tr>";
		out << "<td style=\"font-weight:bold;border: 2px solid black\">";
		out << r->team();
		out << "</td>";
		for (const std::string& name : doubleColumns)
		{
			int col = r->doubleColumn(name);
			double d = r->doubleValue(col);
			out << "<td style=\"border: 2px solid black;\">";
			out << d;
			out << "</td>";
		}

		for (const std::string& name : distColumns)
		{
			int col = r->distColumn(name);
			auto d = r->distValue(col);
			out << "<td style=\"border: 2px solid black;\">";
			const auto& data = d.data();
			int count = 0;
			bool first = true;
			for (auto pair : data)
			{
				out << "<nobr>";
				out << "(" << pair.first << ": " << pair.second << ")";
				count++;

				if (count != data.size())
					out << "    ";

				out << "</nobr>";

				if ((count % 3) == 0)
					out << "<br>";

				first = false;


			}
			out << "</td>";
		}

		out << "</tr>";
	}

	out << "</table>";

	return true;
}

bool OutputFormatter::outputPicklist(int team, const std::string& filename, const std::vector<Pick>& picks, OutputType type)
{
	bool ret = false;

	if (type == OutputType::CSV)
		ret = outputPicklistCSV(team, filename, picks);
	else
		ret = outputPicklistHTML(team, filename, picks);

	return ret;
}

bool OutputFormatter::outputPicklistCSV(int team, const std::string& filename, const std::vector<Pick>& picks)
{
	std::ofstream out(filename);
	if (!out.is_open())
		return false;

	out << "Rank";
	out << ",Team";
	out << ",Score";
	for (int i = 0; i < 22; i++)
	{
		out << "," << i + 1;
		out << ",Score_" << i + 1;
	}
	out << std::endl;

	for (int i = 0; i < picks.size(); i++)
	{
		out << i + 1;
		out << "," << picks[i].team();		
		out << "," << picks[i].nominalScore();

		auto sc = picks[i].scores();
		for (int j = 0; j < sc.size() && j < 22 ; j++)
		{
			out << "," << sc[j].first;
			out << "," << sc[j].second;
		}
		out << std::endl;
	}

	return true;
}

bool OutputFormatter::outputPicklistHTML(int team, const std::string& filename, const std::vector<Pick>& picks)
{
	int width = 22;
	int height = 15;

	std::ofstream out(filename);
	if (!out.is_open())
		return false;

	out << "<!DOCTYPE html>";
	out << "<html lang=\"en\">";
	out << "<head>";
	out << "<meta charset=\"utf-8\"/>";
	out << "<title>Team " << std::to_string(team) << " PickList</title>";
	out << "</head>";
	out << "<h1>Team " << std::to_string(team) << " PickList</h1>";
	out << "<table style=\"border-collapse: collapse;\">";

	// Table header one
	out << "<tr>";
	out << "<th colspan=\"2\" style=\"font-weight:bold;text-align:center; border: 2px solid black\">";
	out << "<b>";
	out << "First pick";
	out << "</b>";
	out << "</th>";
	out << "<th colspan=\"22\" style=\"font-weight:bold;text-align:center; border: 2px solid black\">";
	out << "<b>";
	out << "Second pick";
	out << "</b>";
	out << "</th>";
	out << "</tr>";

	// Table header two
	out << "<tr>";
	out << "<th style=\"font-weight:bold;border: 2px solid black;\">Rank</th>";
	out << "<th style=\"font-weight:bold;border: 2px solid black\">Team</th>";
	for (int i = 0; i < 22; i++)
		out << "<th style=\"font-weight:bold;border: 2px solid black\">" << i + 1 << "</th>";
	out << "</tr>";

	//
	// Now one row per pick
	//
	for (int i = 0; i < picks.size() && i < 15 ; i++)
	{
		const Pick& pick = picks[i];
		auto& s = pick.scores();

		out << "<tr>";
		out << "<th style=\"border: 2px solid black\">";
		out << i + 1;
		out << "</th>";

		out << "<td style=\"border: 2px solid black\">";
		out << pick.team();
		out << "<br>";
		out << "<small>";
		out << pick.nominalScore();
		out << "</small>";
		out << "</td>";

		for (int j = 0; j < width; j++)
		{
			out << "<td style=\"text-align:center; border: 2px solid black\">";
			out << s[j].first;
			out << "<br>";
			out << "<small>";
			out << s[j].second ;
			out << "</small>";
			out << "</td>";
		}
		out << "</tr>";
	}

	out << "</table>";
	out << "</html>";

	return true;
}
