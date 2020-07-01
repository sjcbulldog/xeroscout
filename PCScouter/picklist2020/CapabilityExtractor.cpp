#include "CapabilityExtractor.h"
#include "RobotCapabilities.h"
#include "AllianceGroup.h"
#include <set>
#include <iostream>

CapabilityExtractor::CapabilityExtractor(const Headers& headers, const DataArray& data) : headers_(headers), data_(data)
{
}

CapabilityExtractor::~CapabilityExtractor()
{
	for (auto r : results_)
		delete r;

	results_.clear();
}

bool CapabilityExtractor::createResults()
{
	for (int i = 0; i < data_.size(); i++)
	{
		Result* r = new Result(headers_, data_[i]);
		results_.push_back(r);
	}

	return true;
}

void CapabilityExtractor::addMatchToTeam(int team, AllianceGroup* gr)
{
	auto it = team_matches_.find(team);
	if (it == team_matches_.end())
	{
		std::list<AllianceGroup*> list;
		list.push_back(gr);
		team_matches_.insert_or_assign(team, list);
	}
	else
	{
		it->second.push_back(gr);
	}
}

void CapabilityExtractor::addResultToTeam(int team, Result* r)
{
	auto it = team_results_.find(team);
	if (it == team_results_.end())
	{
		std::list<Result*> list;
		list.push_back(r);
		team_results_.insert_or_assign(team, list);
	}
	else
	{
		it->second.push_back(r);
	}
}

bool CapabilityExtractor::createMatches()
{
	const int BlueOffset = 1000;
	std::map<int, AllianceGroup*> matches;
	std::set<int> done;

	for (auto r : results_)
	{
		addResultToTeam(r->team(), r);

		int mapno = r->match();
		if (r->alliance() == Alliance::Blue)
			mapno += BlueOffset;

		if (done.find(mapno) != done.end())
		{
			//
			// This match was done but we found more data, something is wrong with the data
			//
			std::cerr << "TeamPicker: match " << r->match() << ", Alliance " << toString(r->alliance());
			std::cerr << " has too many entries in the data" << std::endl;
			return false;
		}

		auto it = matches.find(mapno);
		if (it == matches.end())
		{
			AllianceGroup* group = new AllianceGroup(r->match(), r->alliance());
			group->addResult(r);

			matches.insert_or_assign(mapno, group);
			matches_.push_back(group);
		}
		else
		{
			it->second->addResult(r);
			if (it->second->size() == 3)
			{
				for (size_t i = 0; i < 3; i++)
				{
					auto res = it->second->result(i);
					addMatchToTeam(res->team(), it->second);
				}

				done.insert(mapno);
				matches.erase(it);
			}
		}
	}

	return true;
}