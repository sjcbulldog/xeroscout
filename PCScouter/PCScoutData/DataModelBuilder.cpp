//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#include "DataModelBuilder.h"
#include "DataModelMatch.h"
#include "Alliance.h"
#include "OPRCalculator.h"

using namespace xero::scouting::datamodel;
using namespace xero::ba;

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


void DataModelBuilder::breakoutBlueAlliancePerRobotData(std::shared_ptr<ScoutingDataModel> dm, std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>>& data, int maxmatch)
{
	for (auto m : dm->matches()) {
		if (m->match() <= maxmatch)
		{
			breakOutBAData(dm, m, Alliance::Red, data[m->key()].first);
			breakOutBAData(dm, m, Alliance::Blue, data[m->key()].second);
		}
	}
}

void DataModelBuilder::breakOutBAData(std::shared_ptr<ScoutingDataModel> dm, std::shared_ptr<const DataModelMatch> m, Alliance c, ScoutingDataMapPtr data)
{
	ScoutingDataMapPtr newdata1 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata2 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata3 = std::make_shared<ScoutingDataMap>();

	for (auto pair : *data)
	{
		if (pair.first.endsWith("Robot1")) {
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata1->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot2"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata2->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot3"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata3->insert_or_assign(name, pair.second);
		}
		else
		{
			newdata1->insert_or_assign(pair.first, pair.second);
			newdata2->insert_or_assign(pair.first, pair.second);
			newdata3->insert_or_assign(pair.first, pair.second);
		}
	}

	newdata1->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));
	newdata2->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));
	newdata3->insert_or_assign(DataModelMatch::BlueAllianceDataField, QVariant(true));

	dm->addMatchExtraData(m->key(), c, 1, newdata1);
	dm->addMatchExtraData(m->key(), c, 2, newdata2);
	dm->addMatchExtraData(m->key(), c, 3, newdata3);

	dm->addMatchExtraDataFields(newdata1);
	dm->addMatchExtraDataFields(newdata2);
	dm->addMatchExtraDataFields(newdata3);
}

void DataModelBuilder::addBlueAllianceData(std::shared_ptr<BlueAlliance> ba, std::shared_ptr<ScoutingDataModel> dm, int maxmatch)
{
	//
	// Move the data from the blue alliance engine to the data model
	//
	std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>> badata;

	auto matches = ba->getEngine().matches();
	for (auto pair : matches) {
		auto m = dm->findMatchByKey(pair.first);
		if (m == nullptr)
			continue;

		if (!pair.second->scoreBreakdown().isEmpty())
		{
			if (m->match() <= maxmatch)
			{
				auto redmap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "red", redmap);

				auto bluemap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "blue", bluemap);

				badata.insert_or_assign(m->key(), std::make_pair(redmap, bluemap));
			}
		}

		if (pair.second->hasVideos())
		{
			dm->assignVideos(m->key(), pair.second->videos());
		}
	}

	breakoutBlueAlliancePerRobotData(dm, badata, maxmatch);

	OPRCalculator calc(dm);
	calc.calc();
}

bool DataModelBuilder::addTeamsMatches(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<::ScoutingDataModel> dm)
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

			dm->addTeam(it->second->key(), it->second->num(), it->second->nick(), it->second->name(), it->second->city(), it->second->state(), it->second->country());
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


bool DataModelBuilder::addTeams(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<::ScoutingDataModel> dm, const QStringList &teamlist)
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

		dm->addTeam(it->second->key(), it->second->num(), it->second->nick(), it->second->name(), it->second->city(), it->second->state(), it->second->country());
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
std::shared_ptr<::ScoutingDataModel> 
	DataModelBuilder::buildModel(xero::ba::BlueAllianceEngine& engine, 
	std::shared_ptr<::ScoutingForm> team,
	std::shared_ptr<::ScoutingForm> match,
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
		dm->addTeam(key, num, name, it->second->name(), it->second->city(), it->second->state(), it->second->country());
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
std::shared_ptr<::ScoutingDataModel>
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
std::shared_ptr<::ScoutingDataModel>
DataModelBuilder::buildModel(xero::ba::BlueAllianceEngine& engine,
	std::shared_ptr<const ::ScoutingForm> cpit,
	std::shared_ptr<const ::ScoutingForm> cmatch,
	const QString& evkey, QString& error)
{
	std::shared_ptr<ScoutingForm> team = std::make_shared<ScoutingForm>(cpit->obj());
	std::shared_ptr<ScoutingForm> match = std::make_shared<ScoutingForm>(cmatch->obj());

	return DataModelBuilder::buildModel(engine, team, match, evkey, error);
}

//
// Create a new track object
//
std::shared_ptr<RobotTrack> DataModelBuilder::createTrack(std::shared_ptr<ScoutingDataModel> model, const QString& mkey, const QString& tkey)
{
	Alliance c;
	int slot;
	QString color;

	auto m = model->findMatchByKey(mkey);
	if (m == nullptr)
		return nullptr;

	if (!m->teamToAllianceSlot(tkey, c, slot))
		return nullptr;

	auto team = model->findTeamByKey(tkey);
	if (team == nullptr)
		return nullptr;

	color = toString(c);

	if (!m->hasZebra())
		return nullptr;

	const QJsonObject& obj = m->zebra();
	if (!obj.contains("times") || !obj.value("times").isArray())
		return nullptr;

	if (!obj.contains("alliances") || !obj.value("alliances").isObject())
		return nullptr;

	QJsonObject aobj = obj.value("alliances").toObject();

	if (!aobj.contains(color) || !aobj.value(color).isArray())
		return nullptr;

	QJsonArray array = aobj.value(color).toArray();
	if (array.size() != 3)
		return nullptr;

	auto t = std::make_shared<RobotTrack>(mkey, tkey, c);
	if (!extractOneAlliance(array, slot - 1, t))
		return nullptr;

	QJsonArray tarray = obj.value("times").toArray();
	getTimes(tarray, t);

	return t;
}

void DataModelBuilder::getTimes(const QJsonArray& array, std::shared_ptr<RobotTrack> track)
{
	for (int i = 0; i < array.size(); i++)
	{
		if (array[i].isDouble())
		{
			double d = array[i].toDouble();
			track->addTime(d);
		}
	}
}

bool DataModelBuilder::extractOneAlliance(const QJsonArray& arr, int index, std::shared_ptr<xero::scouting::datamodel::RobotTrack> track)
{
	if (!arr[index].isObject())
		return false;

	const QJsonObject& obj = arr[index].toObject();

	if (!obj.contains("xs") || !obj.value("xs").isArray())
		return false;

	if (!obj.contains("ys") || !obj.value("ys").isArray())
		return false;

	const QJsonArray& xa = obj.value("xs").toArray();
	const QJsonArray& ya = obj.value("ys").toArray();

	if (xa.size() != ya.size())
		return false;

	for (int k = 0; k < xa.size(); k++)
	{
		if (xa[k].isUndefined() || ya[k].isUndefined())
			continue;

		if (xa[k].isNull() || ya[k].isNull())
			continue;

		if (!xa[k].isDouble() || !ya[k].isDouble())
			continue;

		double xv = xa[k].toDouble();
		double yv = ya[k].toDouble();

		track->addPoint(QPointF(xv, yv));
	}
	return true;
}