//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#include "DataModelBuilder.h"

using namespace xero::scouting::datamodel;

//
// Read values from a JSON property and convert to a scouting data map.  Basically convert to a set of 
// name value pairs where the values are QVariants which means they can be strings, ints, doubles, or
// booleans
//
void DataModelBuilder::jsonToPropMap(const QJsonObject& obj, const QString& alliance, ScoutingDataMapPtr map)
{
	if (!obj.contains(alliance) || !obj.value(alliance).isObject())
		return;

	QJsonObject propobj = obj.value(alliance).toObject();
	auto keys = propobj.keys();
	for (auto key : keys)
	{
		if (propobj[key].isString())
		{
			map->insert(std::make_pair("ba_" + key, QVariant(propobj[key].toString())));
		}
		else if (propobj[key].isDouble())
		{
			map->insert(std::make_pair("ba_" + key, QVariant(propobj[key].toInt())));
		}
		else if (propobj[key].isBool())
		{
			map->insert(std::make_pair("ba_" + key, QVariant(propobj[key].toBool())));
		}
	}
}

bool DataModelBuilder::addTeamsMatches(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm)
{
	bool assignteams = false;

	auto it = engine.events().find(dm->evkey());
	if (it == engine.events().end())
		return false;

	dm->blockSignals(true);

	auto matches = engine.matches();

	if (dm->teams().size() == 0)
	{
		assignteams = true;

		QStringList teamlist;
		auto ev = it->second;
		for (auto pair : matches)
		{
			for (auto teamkey : pair.second->blue()->getTeams())
			{
				if (!teamlist.contains(teamkey))
					teamlist.push_back(teamkey);
			}

			for (auto teamkey : pair.second->red()->getTeams())
			{
				if (!teamlist.contains(teamkey))
					teamlist.push_back(teamkey);
			}
		}

		auto teams = engine.teams();
		for (const QString& teamkey : teamlist) {
			auto it = teams.find(teamkey);
			if (it == teams.end())
				return false;

			dm->addTeam(it->second->key(), it->second->num(), it->second->nick());
		}
	}

	for (auto pair : matches)
	{
		auto m = pair.second;
		std::shared_ptr<DataModelMatch> mptr = dm->addMatch(m->key(), m->compLevel(), m->setNumber(), m->matchNumber(), m->eTime());

		QStringList& red = m->red()->getTeams();
		for (int i = 0; i < red.size(); i++) {
			QString team = m->red()->getTeams().at(i);
			auto t = dm->findTeamByKey(team);
			dm->addTeamToMatch(mptr->key(), Alliance::Red, i + 1, team, t->number());
		}

		QStringList& blue = m->blue()->getTeams();
		for (int i = 0; i < blue.size(); i++) {
			QString team = m->blue()->getTeams().at(i);
			auto t = dm->findTeamByKey(team);
			dm->addTeamToMatch(mptr->key(), Alliance::Blue, i + 1, team, t->number());
		}
	}


	// Assign tablets to matches
	dm->assignMatches();

	if (assignteams)
	{
		// Assign tablets to team pits
		dm->assignTeams();
	}

	dm->blockSignals(false);

	return true;
}


bool DataModelBuilder::addTeams(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QStringList &teamlist)
{
	auto it = engine.events().find(dm->evkey());
	if (it == engine.events().end())
		return false;

	dm->blockSignals(true);

	auto teams = engine.teams();
	for (const QString& teamkey : teamlist) {
		auto it = teams.find(teamkey);
		if (it == teams.end())
			return false;

		dm->addTeam(it->second->key(), it->second->num(), it->second->nick());
	}

	// Assign tablets to team pits
	dm->assignTeams();

	dm->blockSignals(false);

	return true;
}

//
// Build a data model for an event from blue alliance data.  In this case we are given
// scouting forms that are ours to own.
//
std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> 
	DataModelBuilder::buildModel(xero::ba::BlueAllianceEngine& engine, 
	std::shared_ptr<xero::scouting::datamodel::ScoutingForm> team,
	std::shared_ptr<xero::scouting::datamodel::ScoutingForm> match,
		const QString& evkey, QString& error)
{
	auto& bateams = engine.teams();
	auto& bamatches = engine.matches();

	auto evit = engine.events().find(evkey);
	if (evit == engine.events().end())
	{
		error = "no event with the key '" + evkey + "' exists";
		return nullptr;
	}
	auto ev = evit->second;

	auto dm = std::make_shared<ScoutingDataModel>(ev->key(), ev->name(), ev->start(), ev->end());
	QStringList tags;
	if (!dm->setScoutingForms(team, match, tags)) {
		error = "duplicate tags '";
		for (int i = 0; i < tags.size(); i++)
		{
			if (i != 0)
				error += ", ";

			error += tags.at(i);
		}
		error += "' exist across the team scouting form and the match scouting form";
		return nullptr;
	}

	QStringList teams;

	for (auto m : ev->matches())
	{
		for (const QString& t : m->red()->getTeams())
		{
			if (!teams.contains(t))
				teams.push_back(t);
		}

		for (const QString& t : m->blue()->getTeams())
		{
			if (!teams.contains(t))
				teams.push_back(t);
		}
	}

	for (const QString& team : teams)
	{
		auto it = bateams.find(team);
		if (it == bateams.end())
			continue;

		QString key = it->second->key();
		int num = it->second->num();
		QString name = it->second->name();
		dm->addTeam(key, num, name);
	}

	for (auto m : ev->matches())
	{
		auto match = dm->addMatch(m->key(), m->compLevel(), m->setNumber(), m->matchNumber(), m->pTime());

		QStringList& red = m->red()->getTeams();
		for (int i = 0; i < red.size(); i++) {
			QString team = m->red()->getTeams().at(i);
			auto t = dm->findTeamByKey(team);
			dm->addTeamToMatch(match->key(), Alliance::Red, i + 1, team, t->number());
		}

		QStringList& blue = m->blue()->getTeams();
		for (int i = 0; i < blue.size(); i++) {
			QString team = m->blue()->getTeams().at(i);
			auto t = dm->findTeamByKey(team);
			dm->addTeamToMatch(match->key(), Alliance::Blue, i + 1, team, t->number());
		}
	}

	return dm;
}

//
// Build a data model for an event from blue alliance data.  In this case we are given
// the name of files that contains the scouting forms
//
std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
DataModelBuilder::buildModel(xero::ba::BlueAllianceEngine& engine,
	const QString& pitform, const QString& matchform,
	const QString& evkey, QString& error)
{
	std::shared_ptr<ScoutingForm> team = std::make_shared<ScoutingForm>(pitform);
	std::shared_ptr<ScoutingForm> match = std::make_shared<ScoutingForm>(matchform);

	return DataModelBuilder::buildModel(engine, team, match, evkey, error);
}

//
// Build a data model for an event from blue alliance data.  In this case we are given
// pointers to scouting forms that are not ours and we need to make a copy.
//
std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
DataModelBuilder::buildModel(xero::ba::BlueAllianceEngine& engine,
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> cpit,
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> cmatch,
	const QString& evkey, QString& error)
{
	std::shared_ptr<ScoutingForm> team = std::make_shared<ScoutingForm>(cpit->obj());
	std::shared_ptr<ScoutingForm> match = std::make_shared<ScoutingForm>(cmatch->obj());

	return DataModelBuilder::buildModel(engine, team, match, evkey, error);
}
