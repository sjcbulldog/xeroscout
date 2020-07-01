#include "PickListFormatter.h"
#include <fstream>

bool PickListFormatter::outputData(int team, const std::string& filename, const std::vector<Pick>& picks, OutputType type)
{
	bool ret = false;

	if (type == OutputType::CSV)
		ret = outputDataCSV(team, filename, picks);
	else
		ret = outputDataHTML(team, filename, picks);

	return ret;
}

bool PickListFormatter::outputDataCSV(int team, const std::string& filename, const std::vector<Pick>& picks)
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
		out << ",Score";
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

bool PickListFormatter::outputDataHTML(int team, const std::string& filename, const std::vector<Pick>& picks)
{
	return false;
}

#ifdef NOTYET
void show_picklist(Team picker, Picklist const& a) {
	auto heading = "Team " + as_string(picker) + " Picklist";

	auto show_box = [](std::pair<double, Team> p)->std::string {
		return td(
			as_string(p.second)
			+ "<br>"
			+ tag(
				"small",
				tag("font color=grey", p.first)
			)
		);
	};

	auto s = html(
		head(title(heading))
		+ body(
			h1(heading) +
			tag("table border",
				tr(
					tag("th colspan=2", "First pick") +
					tag("th colspan=22", "Second pick")
				) +
				tr(
					th("Rank") +
					th("Team") +
					join(mapf([](auto i) { return th(i); }, range(1, 23)))
				) +
				join(mapf(
					[=](auto p) {
						auto [i, x] = p;
						auto [fp, second] = x;
						return tr(
							th(i)
							+ show_box(fp)
							+ join(mapf(show_box, take(22, second)))
						);
					},
					enumerate_from(1, take(15, a))
						))
			)
		)
	);
	write_file("picklist.html", s);
}

#endif