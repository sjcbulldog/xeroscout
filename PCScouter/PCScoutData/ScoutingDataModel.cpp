//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "ScoutingDataModel.h"
#include "ScoutingDatabase.h"
#include "TabletAssigner.h"
#include "ScoutDataJsonNames.h"
#include "JSonDataModelConverter.h"
#include "TeamDataSummary.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QDebug>
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ScoutingDataModel::ScoutingDataModel(const QString& evkey, const QString& evname, std::shared_ptr<ScoutingForm> team, std::shared_ptr<ScoutingForm> match)
			{
				ev_key_ = evkey;
				event_name_ = evname;

				filename_ = QStandardPaths::locate(QStandardPaths::DocumentsLocation, ev_key_);

				team_scouting_form_ = team;

				match_scouting_form_ = match;

				dirty_ = false;

				emit_changes_ = true;
			}

			ScoutingDataModel::ScoutingDataModel()
			{
				dirty_ = false;
				emit_changes_ = true;
			}

			ScoutingDataModel::~ScoutingDataModel()
			{
			}

			bool ScoutingDataModel::isBlueAllianceDataLoaded() const
			{
				for (auto m : matches_)
				{
					if (m->hasBlueAllianceData())
						return true;
				}

				return false;
			}

			bool ScoutingDataModel::createTeamSummaryData(const QString& key, TeamDataSummary& result)
			{
				ScoutingDataSet ds;
				QString err;
				QString query = "select * from matches where TeamKey='" + key + "'";

				auto team = findTeamByKey(key);

				if (!createCustomDataSet(ds, query, err))
					return false;

				result.clear();
				result.processDataSet(ds);

				return true;
			}

			bool ScoutingDataModel::load(const QString& filename)
			{
				filename_ = filename;

				QFile file(filename_);
				if (!file.open(QIODevice::ReadOnly))
					return false;

				QByteArray data = file.readAll();
				QJsonParseError err;
				QJsonDocument doc;
				
				if (filename_.endsWith(".evd"))
					doc = QJsonDocument::fromBinaryData(data);
				else
					doc = QJsonDocument::fromJson(data, &err);

				if (doc.isNull())
					return false;

				emit_changes_ = false;
				JSonDataModelConverter conv(this);
				if (!conv.allFromJson(doc))
				{
					emit_changes_ = true;
					reset();
					return false;
				}

				emit_changes_ = true;
				dirty_ = false;
				return true;
			}

			bool ScoutingDataModel::loadScoutingDataJSON(const QString& filename)
			{
				filename_ = filename;

				QFile file(filename_);
				if (!file.open(QIODevice::ReadOnly))
					return false;

				QByteArray data = file.readAll();
				QJsonParseError err;
				QJsonDocument doc;
				
				if (filename_.endsWith(".evd"))
					doc = QJsonDocument::fromBinaryData(data);
				else
					doc = QJsonDocument::fromJson(data, &err);

				if (doc.isNull())
					return false;

				emit_changes_ = false;
				JSonDataModelConverter conv(this);
				if (!conv.scoutingFromAllJson(doc))
				{
					emit_changes_ = true;
					reset();
					return false;
				}

				emit_changes_ = true;
				dirty_ = false;
				return true;
			}

			QJsonDocument ScoutingDataModel::generateAllJSONDocument()
			{
				JSonDataModelConverter conv(this);
				return conv.allToJsonDoc();
			}

			QJsonDocument ScoutingDataModel::generateCoreJSONDocument(bool extra)
			{
				JSonDataModelConverter conv(this);
				return conv.coreToJsonDoc(extra);
			}

			QJsonDocument ScoutingDataModel::generateScoutingData(const TabletIdentity* sender, const QString& tablet)
			{
				JSonDataModelConverter conv(this);
				return conv.scoutingToJsonDoc(sender, tablet);
			}

			bool ScoutingDataModel::loadAllJSON(const QJsonDocument& doc, QStringList& error)
			{
				JSonDataModelConverter conv(this);

				if (!conv.allFromJson(doc))
				{
					error = conv.errors();
					return false;
				}

				return true;
			}

			bool ScoutingDataModel::loadCoreJSON(const QJsonDocument& doc, QStringList& error)
			{
				JSonDataModelConverter conv(this);

				if (!conv.coreFromJson(doc))
				{
					error = conv.errors();
					return false;
				}

				return true;
			}

			bool ScoutingDataModel::loadScoutingDataJSON(const QJsonDocument& doc, TabletIdentity& id, QString& error, QStringList& pits, QStringList& matches)
			{
				JSonDataModelConverter conv(this);

				if (!conv.scoutingFromJson(doc, id, pits, matches))
				{
					error = conv.errors().front();
					return false;
				}

				SyncHistoryRecord rec(id, QDateTime::currentDateTime(), pits, matches);
				addHistoryRecord(rec);

				return true;
			}

			int ScoutingDataModel::teamKeyToNumber(const QString& key) {
				for (auto t : teams_) {
					if (t->key() == key)
						return t->number();
				}

				return -1;
			}

			void ScoutingDataModel::assignMatches()
			{
				matches_.sort([](std::shared_ptr<DataModelMatch> a, std::shared_ptr<DataModelMatch> b) -> bool
					{
						return a->eventTime() < b->eventTime();
					});

				TabletAssigner a(match_tablets_);
				a.assign(static_cast<int>(matches_.size()), TabletAssigner::Strategy::Simple);

				assert(a.count() == matches_.size());

				int matchno = 0;
				for (auto dm : matches_) {
					int index = 0;
					const QStringList& matchassign = a.match(matchno++);

					for (int i = 1; i <= 3; i++) {
						dm->setTablet(Alliance::Red, i, matchassign[index++]);
					}
					for (int i = 1; i <= 3; i++) {
						dm->setTablet(Alliance::Blue, i, matchassign[index++]);
					}
				}

				emitChangedSignal(ChangeType::MatchScoutingTabletChanged);
			}

			void ScoutingDataModel::assignPits()
			{
				int tablet = 0;

				for (auto team : teams_) {
					team->setTablet(pit_tablets_[tablet]);
					incrPitTabletIndex(tablet);
				}

				emitChangedSignal(ChangeType::PitScoutingTabletChanged);
			}

			std::shared_ptr<DataModelMatch> ScoutingDataModel::findMatch(const QString& comp, int set, int match)
			{
				for (auto m : matches_) {
					if (m->compType() == comp && m->set() == set && m->match() == match)
						return m;
				}

				return nullptr;
			}

			void ScoutingDataModel::processDataSetAlliance(ScoutingDataSet& set, std::shared_ptr<DataModelMatch> m, Alliance c) const
			{
				auto scouting_form_fields = match_scouting_form_->fields();
				QVariant vinvalid;

				for (int i = 1; i <= 3; i++)
				{
					QString tkey = m->team(c, i);
					auto team = findTeamByKey(tkey);

					if (team == nullptr)
						continue;

					set.newRow();

					set.addData(m->compType());
					set.addData(m->set());
					set.addData(m->match());
					set.addData(m->key());

					set.addData(team->number());
					set.addData(tkey);
					set.addData(DataModelMatch::toString(c));
					set.addData(i);

					auto data = m->scoutingData(c, i);
					if (data == nullptr) {
						for (int i = 0; i < scouting_form_fields.size(); i++)
							set.addData(vinvalid);
					}
					else {
						for (int i = 0; i < scouting_form_fields.size(); i++)
						{
							auto it = data->find(scouting_form_fields[i].first);
							if (it == data->end()) {
								qDebug() << "createMatchDataset: missing field entry '" << scouting_form_fields[i].first << "'";
								set.addData(vinvalid);
							}
							else
							{
								set.addData(it->second);
							}
						}
					}

					auto ba = m->blueAllianceData(c, i);
					if (ba != nullptr) {
						for (auto pair : *ba)
							set.addData(pair.second);
					}
				}
			}

			void ScoutingDataModel::createMatchDataSet(ScoutingDataSet& set) const
			{
				auto scouting_form_fields = match_scouting_form_->fields();
				QVariant vinvalid;

				set.clear();

				//
				// Generate the headers
				//
				set.addHeader("Type", QVariant::Type::String);
				set.addHeader("SetNumber", QVariant::Type::Int);
				set.addHeader("MatchNumber", QVariant::Type::Int);
				set.addHeader("MatchKey", QVariant::Type::String);
				set.addHeader("TeamNumber", QVariant::Type::Int);
				set.addHeader("TeamKey", QVariant::Type::String);
				set.addHeader("Alliance", QVariant::Type::String);
				set.addHeader("Position", QVariant::Type::String);

				for (int f = 0; f < scouting_form_fields.size(); f++) {
					set.addHeader(scouting_form_fields[f].first, scouting_form_fields[f].second);
				}

				// Color does not matter here, we just want the headers
				auto badata = matches_.front()->blueAllianceData(Alliance::Red, 1);
				if (badata != nullptr)
				{
					for (auto entry : *badata)
						set.addHeader(entry.first, entry.second.type());
				}

				//
				// Generate the data
				//
				for (auto m : matches_) {
					processDataSetAlliance(set, m, Alliance::Red);
					processDataSetAlliance(set, m, Alliance::Blue);
				}
			}

			void ScoutingDataModel::createTeamDataSet(ScoutingDataSet& set) const
			{
				QVariant invalid;
				auto fields = team_scouting_form_->fields();
				QStringList extra;

				set.clear();
				set.addHeader("Team", QVariant::String);
				set.addHeader("TeamNumber", QVariant::Int);
				set.addHeader("TeamKey", QVariant::String);

				for (int f = 0; f < fields.size(); f++) {
					set.addHeader(fields[f].first, fields[f].second);
				}

				auto ex = teams_.front()->extraData();
				if (ex != nullptr)
				{
					for (auto pair : *ex)
					{
						set.addHeader(pair.first, QVariant::Type::Double);
						extra.push_back(pair.first);
					}
				}

				auto teams = teams_;
				teams.sort([](std::shared_ptr<DataModelTeam> a, std::shared_ptr<DataModelTeam> b) -> bool
					{
						return a->number() < b->number();
					});

				for (auto t : teams)
				{
					set.newRow();

					set.addData(t->name());
					set.addData(t->number());
					set.addData(t->key());

					auto data = t->pitScoutingData();
					if (data == nullptr)
					{
						for (int i = 0; i < fields.size(); i++)
							set.addData(invalid);
					}
					else
					{
						for (int i = 0; i < fields.size(); i++)
						{
							auto it = data->find(fields[i].first);
							if (it == data->end()) {
								qDebug() << "createPitDataSet: missing field '" << fields[i].first << "'";
								set.addData(invalid);
							}
							else
							{
								set.addData(it->second);
							}
						}
					}

					ex = t->extraData();
					if (ex != nullptr)
					{
						for (auto exname : extra)
						{
							auto it = ex->find(exname);
							set.addData(it->second);
						}
					}
				}
			}

			bool ScoutingDataModel::createCustomDataSet(ScoutingDataSet& set, const QString& query, QString& error) const
			{
				ScoutingDatabase* db = ScoutingDatabase::getInstance();
				if (db == nullptr)
					return false;

				if (!db->hasTable("matches"))
				{
					ScoutingDataSet matchset;
					createMatchDataSet(matchset);
					if (!db->addTable("matches", matchset))
						return false;
				}

				if (!db->hasTable("teams"))
				{
					ScoutingDataSet pitset;
					createTeamDataSet(pitset);
					if (!db->addTable("teams", pitset))
						return false;
				}

				if (!db->executeQuery(query, set, error))
					return false;

				return true;
			}


			ScoutingDataMapPtr ScoutingDataModel::generateRandomData(GameRandomProfile& gen, std::shared_ptr<ScoutingForm> form)
			{
				std::shared_ptr<ScoutingDataMap> data = std::make_shared<ScoutingDataMap>();

				for (auto section : form->sections())
				{
					for (auto item : section->items())
					{
						DataCollection coll = item->random(gen);
						for (int i = 0; i < coll.count(); i++)
						{
							data->insert_or_assign(coll.tag(i), coll.data(i));
						}
					}
				}

				return data;
			}

			void ScoutingDataModel::generateRandomScoutingData(GameRandomProfile& profile, int mod)
			{
				int cnt = 0;
				for (auto team : teams_)
				{
					if ((cnt % mod) == 0)
						team->setPitScoutingData(generateRandomData(profile, team_scouting_form_), true);

					cnt++;
				}

				cnt = 0;
				for (auto match : matches_)
				{
					for (int i = 1; i <= 3; i++)
					{
						if ((cnt % mod) == 0)
							match->setScoutingData(Alliance::Red, i, generateRandomData(profile, match_scouting_form_), true);
						cnt++;

						if ((cnt % mod) == 0)
							match->setScoutingData(Alliance::Blue, i, generateRandomData(profile, match_scouting_form_), true);
						cnt++;
					}
				}

				emitChangedSignal(ChangeType::PitScoutingDataAdded);
				emitChangedSignal(ChangeType::MatchScoutingDataAdded);
			}

			void ScoutingDataModel::breakOutBAData(std::shared_ptr<DataModelMatch> m, Alliance c, ScoutingDataMapPtr data)
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

				m->setBlueAllianceData(c, 1, newdata1);
				m->setBlueAllianceData(c, 2, newdata2);
				m->setBlueAllianceData(c, 3, newdata3);
			}

			void ScoutingDataModel::breakoutBlueAlliancePerRobotData(std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>>& data)
			{
				for (auto m : matches_) {
					breakOutBAData(m, Alliance::Red, data[m->key()].first);
					breakOutBAData(m, Alliance::Blue, data[m->key()].second);
				}
			}

			void ScoutingDataModel::removeScoutingData(const QString& tablet)
			{
				for (auto m : matches_)
				{
					Alliance color;
					int slot;

					if (m->tabletToAllianceSlot(tablet, color, slot))
						m->clearScoutingData(color, slot);
				}

				for (auto t : teams_)
				{
					if (t->tablet() == tablet)
						t->clearPitScoutingData();
				}
			}

			void ScoutingDataModel::removeOldScoutingData()
			{
				for (auto m : matches_)
				{
					m->removeOldScoutingData();
				}

				for (auto t : teams_)
				{
					t->removeOldScoutingData();
				}
			}

			QStringList ScoutingDataModel::getAllFieldNames() const
			{
				QStringList list;

				list = getMatchFieldNames();

				list.append(getTeamFieldNames());

				return list;
			}

			QStringList ScoutingDataModel::getTeamFieldNames() const
			{
				QStringList list;

				//
				// Get any fields from the team scouting form
				//
				auto flist = team_scouting_form_->fields();
				for (auto f : flist)
					list.push_back(f.first);

				//
				// Make sure the extra data is consistent
				//
				int cnt = -1;
				for (auto team : teams())
				{
					auto ex = team->extraData();
					if (ex != nullptr)
					{
						if (cnt == -1)
							cnt = ex->size();

						assert(cnt == ex->size());
					}
				}

				if (cnt != -1)
				{
					auto ex = teams().front()->extraData();
					if (ex != nullptr)
					{
						for (auto pair : *ex)
							list.push_back(pair.first);
					}
				}

				return list;
			}


			QStringList ScoutingDataModel::getMatchFieldNames() const
			{
				QStringList list;

				auto flist = match_scouting_form_->fields();
				for (auto f : flist)
					list.push_back(f.first);

				auto m = matches().front();
				auto ba = m->blueAllianceData(Alliance::Red, 1);
				if (ba != nullptr)
				{
					for (auto pair : *ba)
						list.push_back(pair.first);
				}

				return list;
			}
		}
	}
}
