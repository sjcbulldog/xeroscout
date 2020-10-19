#include "PicklistHTMLGenerator.h"

using namespace xero::scouting::datamodel;

QString PicklistHTMLGenerator::genPicklistHTML(int team, const std::vector<PickListEntry>& picks, int rows, int cols)
{
	QString out;

	if (picks.size() == 0)
	{
		out = "<h1>No Picklist available";
		return out;
	}

	out += "<!DOCTYPE html>";
	out += "<html lang=\"en\">";
	out += "<head>";
	out += "<meta charset=\"utf-8\"/>";
	out += "<title>Team " + QString::number(team) + " PickList</title>";
	out += "</head>";
	out += "<h1>Team " + QString::number(team) + " PickList</h1>";
	out += "<table style=\"border-collapse: collapse;\">";

	// Table header one
	out += "<tr>";
	out += "<th colspan=\"2\" style=\"font-weight:bold;text-align:center; border: 2px solid black\">";
	out += "<b>";
	out += "First pick";
	out += "</b>";
	out += "</th>";
	out += "<th colspan=\"" + QString::number(cols) + "\" style=\"font-weight:bold;text-align:center; border: 2px solid black\">";
	out += "<b>";
	out += "Second pick";
	out += "</b>";
	out += "</th>";
	out += "</tr>";

	// Table header two
	out += "<tr>";
	out += "<th style=\"font-weight:bold;border: 2px solid black;\">Rank</th>";
	out += "<th style=\"font-weight:bold;border: 2px solid black\">Team</th>";
	for (int i = 0; i < cols; i++)
		out += "<th style=\"font-weight:bold;border: 2px solid black\">" + QString::number(i + 1) + "</th>";
	out += "</tr>";

	//
	// Now one row per pick
	//
	for (int i = 0; i < picks.size() && i < rows ; i++)
	{
		const PickListEntry& pick = picks[i];

		out += "<tr>";
		out += "<th style=\"border: 2px solid black\">";
		out += QString::number(i + 1);
		out += "</th>";

		out += "<td style=\"border: 2px solid black\">";
		out += QString::number(pick.team());
		out += "<br>";
		out += "<small>";
		out += QString::number(pick.score());
		out += "</small>";
		out += "</td>";

		for (int j = 0; j < cols; j++)
		{
			out += "<td style=\"text-align:center; border: 2px solid black\">";
			out += QString::number(pick.thirdTeam(j));
			out += "<br>";
			out += "<small>";
			out += QString::number(pick.thirdScore(j));
			out += "</small>";
			out += "</td>";
		}
		out += "</tr>";
	}

	out += "</table>";
	out += "</html>";

	return out;
}

QString PicklistHTMLGenerator::genRobotCapabilitiesHTML(const std::vector<RobotCapabilities>& robots)
{
	QString out;

	if (robots.size() == 0)
	{
		out = "<h1>No Robot Capabilities available";
		return out;
	}

	const RobotCapabilities& one = robots[0];

	auto doubleColumns = one.doubleColumnNames();
	auto distColumns = one.distColumnNames();

	out += "<!DOCTYPE html>";
	out += "<html lang=\"en\">";
	out += "<head>";
	out += "<meta charset=\"utf-8\"/>";
	out += "<title>Robot Capabilities</title>";
	out += "</head>";
	out += "<h1>Robot Capabilities</h1>";
	out += "<table style=\"border-collapse: collapse;\">";

	out += "<tr>";
	out += "<th style=\"font-weight:bold;border: 2px solid black;\">Team</th>";

	for (const QString & name : doubleColumns)
	{
		out += "<th style=\"font-weight:bold;border: 2px solid black;\">" + name + "</th>";
	}

	for (const QString& name : distColumns)
	{
		out += "<th style=\"font-weight:bold;border: 2px solid black;\">" + name + "</th>";
	}

	out += "</tr>";

	for (auto r : robots)
	{
		out += "<tr>";
		out += "<td style=\"font-weight:bold;border: 2px solid black\">";
		out += QString::number(r.team());
		out += "</td>";

		for (auto pair : r.doubleParams())
		{
			out += "<td style=\"border: 2px solid black;\">";
			out += QString::number(pair.second);
			out += "</td>";
		}

		for (auto pair : r.distParams())
		{
			auto data = pair.second;
			out += "<td style=\"border: 2px solid black;\">";

			int count = 0;
			bool first = true;
			for (auto pair : data)
			{
				out += "<nobr>";
				out += "(" + QString::number(pair.first) + ": " + QString::number(pair.second) + ")";
				count++;

				if (count != data.size())
					out += "    ";

				out += "</nobr>";

				if ((count % 3) == 0)
					out += "<br>";

				first = false;


			}
			out += "</td>";
		}

		out += "</tr>";
	}

	out += "</table>";

	return out;
}