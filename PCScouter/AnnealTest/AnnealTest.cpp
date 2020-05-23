// AnnealTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

typedef std::array<int, 6> Match;
typedef std::vector<std::pair<int, int>> Chain;

static int chain_weight = 4;
static int span_length = 12;
static int long_span_cost = 40;
static std::vector<int> chain_len_weight =
{
	60, 				// chain gap length 1
	60,					// chain gap length 2
	60,	     			// chain gap length 3
	60,  				// chain gap length 4
	40,					// chain gap length 5
	40,					// chain gap length 6
	3,					// chain gap length 7
	3,					// chain gap length 8
	2,					// chain gap length 9
	2,					// chain gap length 10
	2,					// chain gap length 11
	2,					// chain gap length 12
	1					// chain gap length 13 and over
};

static bool prstuff = false;

void initialPlacement(std::vector<Match>& ev, int matches, int tablets)
{
	int t = 0;

	ev.clear();
	for (int i = 0; i < matches; i++) {
		Match m;

		for (int j = 0; j < 6; j++) {
			m[j] = t++;
			if (t == tablets)
				t = 0;
		}
		ev.push_back(m);
	}
}

std::vector<std::pair<int, int>> createChains(const std::vector<Match>& ev, int tablet)
{
	bool inchain = false;
	Chain chain;
	std::pair<int, int> current;

	for (int i = 0; i < ev.size(); i++)
	{
		if (std::find(ev[i].begin(), ev[i].end(), tablet) == ev[i].end())
		{
			// No, not in this match, mark the end of the chain
			if (inchain)
			{
				current.second = i - 1;
				chain.push_back(current);
				inchain = false;
			}
		}
		else
		{
			// Yes in this match, start a new chain
			if (!inchain)
			{
				current.first = i;
				inchain = true;
			}
		}
	}

	if (inchain)
	{
		current.second = static_cast<int>(ev.size() - 1);
		chain.push_back(current);
	}

	return chain;
}

std::vector<Chain> findChains(const std::vector<Match>& ev, int tablets)
{
	std::vector<Chain> chains;

	for (int i = 0; i < tablets; i++)
	{
		chains.push_back(createChains(ev, i));
	}

	return chains;
}

int calcCost(const std::vector<Match> &ev, int tablets)
{
	auto chains = findChains(ev, tablets);
	int cost = 0;

	for (int i = 0; i < chains.size(); i++)
	{
		const Chain& ch = chains[i];
		cost += static_cast<int>(ch.size()) * chain_weight;

		for (int j = 0; j < ch.size() - 1; j++)
		{
			int gap = ch[j].second - ch[j + 1].first;
			if (gap < chain_len_weight.size())
				cost += chain_len_weight[gap];

			if (gap <= 3)
			{
				int l1 = ch[j].second - ch[j].first + 1;
				int l2 = ch[j + 1].second - ch[j + 1].first + 1;
				if (l1 + l2 > span_length)
					cost += long_span_cost;
			}
		}
	}

	return cost;
}

std::vector<int> findpossible(const std::vector<Match>& ev, int tablet)
{
	std::vector<int> possible;

	for (int i = 0; i < ev.size(); i++)
	{
		const Match& m = ev[i];
		auto it = std::find(m.begin(), m.end(), tablet);
		if (it == m.end())
			possible.push_back(i);
	}

	return possible;
}

std::vector<int> findmissing(const Match& m, int tablets)
{
	std::vector<int> missing;

	for (int i = 0; i < tablets; i++) {
		auto it = std::find(m.begin(), m.end(), i);
		if (it == m.end())
			missing.push_back(i);
	}

	return missing;
}

std::vector<int> filterpossible(const std::vector<Match>& ev, const std::vector<int>& possible, const std::vector<int>& missing)
{
	std::vector<int> filtered;

	for (int i = 0; i < possible.size(); i++) {
		for (int j = 0; j < missing.size(); j++) {
			const Match& m = ev[possible[i]];
			auto it = std::find(m.begin(), m.end(), missing[j]);
			if (it != m.end())
			{
				filtered.push_back(possible[i]);
				break;
			}
		}
	}

	return filtered;
}

void dumpMatchSet(const char* title, const std::vector<Match>& ev, const std::vector<int>& set)
{
	if (!prstuff)
		return;

	std::cout << "Dumping Set: " << title << std::endl;

	for (int i = 0; i < set.size(); i++)
	{
		std::cout << std::setw(2) << set[i] << ": ";
		const Match& m = ev[set[i]];
		for (int j = 0; j < m.size(); j++)
		{
			std::cout << std::setw(4) << m[j];
		}
		std::cout << std::endl;
	}
}

void dumpMatch(const char *title, const Match &m)
{
	if (!prstuff)
		return;
	std::cout << title << ": ";
	for (int i = 0; i < m.size(); i++)
		std::cout << std::setw(4) << m[i];
	std::cout << std::endl;
}

void dumpMissing(const std::vector<int>& missing)
{
	if (!prstuff)
		return;

	std::cout << "Missing: ";
	for (int i = 0; i < missing.size(); i++)
		std::cout << std::setw(4) << missing[i];

	std::cout << std::endl;
}

void dumpEvent(const std::vector<Match>& ev)
{
	if (!prstuff)
		return;

	for (int i = 0; i < ev.size(); i++)
	{
		std::cout << std::setw(2) << i << ": ";

		const Match& m = ev[i];
		for (int j = 0; j < m.size(); j++)
			std::cout << std::setw(4) << m[j];
		std::cout << std::endl;
	}
}

void pertubate(std::vector<Match>& ev, int tablets, int count)
{
	// Find the match, slot and tablet we are going to swap (the source)
	int match = rand() % (ev.size());
	int srcslot = rand() % 6;
	int tablet = (ev[match])[srcslot];

	if (prstuff) 
	{
		dumpMatch("Source", ev[match]);
		std::cout << "MatchIndex " << match << ", slot " << srcslot << ", tablet " << tablet << std::endl;
	}

	//
	// The matches that don't include that tablet, as they are the only ones we can swap
	// with (the possible destinations)
	//
	std::vector<int> possible = findpossible(ev, tablet);
	dumpMatchSet("possible", ev, possible);

	//
	// Find the tablets that are missing in the source row
	//
	std::vector<int> missing = findmissing(ev[match], tablets);
	dumpMissing(missing);

	//
	// From the matches that don't include the source tablet, find those
	// that do include the missing tabltes
	//
	std::vector<int> filtered = filterpossible(ev, possible, missing);
	dumpMatchSet("filtered", ev, possible);

	// Ok, the filtered list is the list of matches that are real possibliities, pick on at random
	int which = rand() % filtered.size();

	// Find the two matches that will participate in the swap
	Match& src = ev[match];
	Match& dest = ev[filtered[which]];

	// Find the slot in the destination match
	int destslot = -1;
	for (int i = 0; i < dest.size(); i++)
	{
		auto it = std::find(missing.begin(), missing.end(), dest[i]);
		if (it != missing.end())
		{
			destslot = i;
			break;
		}
	}
	assert(destslot != -1);

	if (prstuff)
	{
		std::cout << "Destination: match " << filtered[which] << ", slot " << destslot << std::endl;
		std::cout << "---------------------------------" << std::endl;
		dumpEvent(ev);
		std::cout << "+++++++++++++++++++++++++++++++++" << std::endl;
	}

	// Now swap the two
	src[srcslot] = dest[destslot];
	dest[destslot] = tablet;
	if (prstuff)
	{
		dumpEvent(ev);
		std::cout << "=================================" << std::endl;
	}
}

void dump(const std::vector<Match>& ev, int tablets)
{
	for (int i = 0; i < ev.size(); i++)
	{
		const Match& m = ev[i];
		for (int j = 0; j < tablets; j++)
		{
			auto it = std::find(m.begin(), m.end(), j);
			if (it == m.end())
				std::cout << ' ';
			else
				std::cout << 'X';
		}
		std::cout << std::endl;
	}
}

void dumpChains(const std::vector<Match>& ev, int tablets)
{
	auto chains = findChains(ev, tablets);

	for (int i = 0; i < chains.size(); i++)
	{
		const Chain & ch = chains[i];

		std::cout << "Tablet " << std::setw(1) << i << ": ";

		if (ch[0].first != 0)
			std::cout << "[off " << ch[0].first << "]";

		for (int j = 0; j < ch.size() ; j++)
		{
			auto pair = ch[j];

			std::cout << "[on " << pair.second - pair.first + 1 << "]";
			if (j != ch.size() - 1)
				std::cout << "[off " << ch[j + 1].first - pair.second - 1 << "]";
		}

		if (ch[ch.size() - 1].second != ev.size() - 1)
			std::cout << "[off " << ev.size() - 1 - ch[ch.size() - 1].second << "]";

	std::cout << std::endl;
	}
}

void anneal(int matches, int tablets, int maxiters, int sameiters)
{
	std::vector<Match> ev;
	int pcnt = 40;
	int cost;
	int newcost;
	int current = 0;

	initialPlacement(ev, matches, tablets);
	cost = calcCost(ev, tablets);

	for(int i = 0 ; i < maxiters; i++)
	{
		std::vector<Match> save = ev;

		pertubate(ev, tablets, pcnt);

		newcost = calcCost(ev, tablets);
		if (newcost >= cost) {
			ev = save;
			current++;

			if (current > sameiters)
				break;
		}
		else {
			if (newcost < cost)
			{
				cost = newcost;
				current = 0;
			}
		}

		if ((i % 10000) == 0 && pcnt > 10)
			pcnt--;

		std::cout << "Cost " << cost << ", for " << current << " iterations, " << pcnt << " pertubations" << '\r' << std::flush;
	}

	dumpChains(ev, tablets);
	std::cout << "Cost " << cost << std::endl;
}

int main()
{
	int seed = static_cast<int>(time(NULL));

	srand(seed);
	anneal(72, 8, 100000000, 50000);
	std::cout << "Seed: " << seed << std::endl;
}