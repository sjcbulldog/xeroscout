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
			ScoutingDataModel::ScoutingDataModel(const QString& evkey, const QString& evname, const QDate &date, const QDate &end)
			{
				ev_key_ = evkey;
				event_name_ = evname;
				start_date_ = date;
				end_date_ = end;
				uuid_ = QUuid::createUuid();

				filename_ = QStandardPaths::locate(QStandardPaths::DocumentsLocation, ev_key_);
				role_ = Role::CentralMachine;
				dirty_ = false;

				emit_changes_ = true;

				addTemporaryFieldDesc();
			}

			ScoutingDataModel::ScoutingDataModel(ScoutingDataModel::Role r)
			{
				role_ = r;
				dirty_ = false;
				emit_changes_ = true;

				addTemporaryFieldDesc();
			}

			ScoutingDataModel::~ScoutingDataModel()
			{
			}

			bool ScoutingDataModel::peekUUID(const QJsonDocument& doc, QUuid& uuid)
			{
				return JSonDataModelConverter::peekUUID(doc, uuid);
			}

			bool ScoutingDataModel::setScoutingForms(std::shared_ptr<ScoutingForm> teamform, std::shared_ptr<ScoutingForm> matchform, QStringList &dups)
			{
				bool ret = true;

				dups.clear();
				//
				// This method should only be used once during initialization
				//
				if (team_scouting_form_ != nullptr || match_scouting_form_ != nullptr)
					return false;

				team_scouting_form_ = teamform;
				match_scouting_form_ = matchform;

				for (auto field : team_scouting_form_->fields())
				{
					if (match_scouting_form_->itemByName(field->name()) != nullptr)
					{
						dups.push_back(field->name());
						ret = false;
					}
				}

				return ret;
			}

			void ScoutingDataModel::addTemporaryFieldDesc()
			{
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::MatchKeyName, FieldDesc::Type::String, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::TypeName, FieldDesc::Type::String, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::SetName, FieldDesc::Type::Integer, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::MatchName, FieldDesc::Type::Integer, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::MatchTeamKeyName, FieldDesc::Type::String, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::TeamNumberName, FieldDesc::Type::Integer, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::AllianceName, FieldDesc::Type::String, false, true));
				match_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelMatch::PositionName, FieldDesc::Type::Integer, false, true));

				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamName, FieldDesc::Type::String, false, true));
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamNumberName, FieldDesc::Type::Integer, false, true));
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamKeyName, FieldDesc::Type::String, false, true));
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

			QJsonDocument ScoutingDataModel::generateZebraData(const QStringList &keys)
			{
				JSonDataModelConverter conv(this);
				return conv.zebraData(keys);
			}


			QJsonDocument ScoutingDataModel::generateMatchDetailData(const QStringList &keys)
			{
				JSonDataModelConverter conv(this);
				return conv.matchDetailData(keys);
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

			bool ScoutingDataModel::loadZebraData(const QJsonDocument& doc)
			{
				JSonDataModelConverter conv(this);

				if (!conv.zebraFromJson(doc))
					return false;

				return true;
			}

			bool ScoutingDataModel::loadMatchDetailData(const QJsonDocument& doc)
			{
				JSonDataModelConverter conv(this);

				if (!conv.matchDetailFromJson(doc))
					return false;

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

			void ScoutingDataModel::assignTeams()
			{
				int tablet = 0;

				for (auto team : teams_) {
					team->setTablet(team_tablets_[tablet]);
					incrTeamTabletIndex(tablet);
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

			std::shared_ptr<FieldDesc> ScoutingDataModel::getFieldByName(const QString& name) const
			{
				for (auto f : match_scouting_form_->fields())
				{
					if (f->name() == name)
						return f;
				}

				for (auto f : team_scouting_form_->fields())
				{
					if (f->name() == name)
						return f;
				}

				for (auto f : match_extra_fields_)
				{
					if (f->name() == name)
						return f;
				}

				for (auto f : team_extra_fields_)
				{
					if (f->name() == name)
						return f;
				}

				return nullptr;
			}

			void ScoutingDataModel::processDataSetAlliance(ScoutingDataSet& set, std::shared_ptr<DataModelMatch> m, Alliance c) const
			{
				for (int slot = 1; slot <= 3; slot++)
				{
					set.newRow();
					for (int i = 0; i < set.columnCount(); i++)
					{
						QVariant v = m->value(c, slot, set.colHeader(i)->name());
						set.addData(v);
					}
				}
			}

			void ScoutingDataModel::createMatchDataSet(ScoutingDataSet& set)
			{
				auto scouting_form_fields = match_scouting_form_->fields();

				set.clear();

				for (auto entry : match_extra_fields_)
				{
					set.addHeader(entry);
				}


				for (int f = 0; f < scouting_form_fields.size(); f++)
				{
					set.addHeader(scouting_form_fields[f]);
				}

				//
				// Generate the data
				//
				for (auto m : matches_) {
					processDataSetAlliance(set, m, Alliance::Red);
					processDataSetAlliance(set, m, Alliance::Blue);
				}
			}

			void ScoutingDataModel::createTeamDataSet(ScoutingDataSet& set)
			{
				std::list<std::shared_ptr<FieldDesc>> newhdrs;
				auto fields = team_scouting_form_->fields();

				set.clear();

				for (auto field : team_extra_fields_)
					set.addHeader(field);

				for (int f = 0; f < fields.size(); f++) {
					set.addHeader(fields[f]);
				}

				auto teams = teams_;
				teams.sort([](std::shared_ptr<DataModelTeam> a, std::shared_ptr<DataModelTeam> b) -> bool
					{
						return a->number() < b->number();
					});

				for (auto t : teams)
				{
					set.newRow();

					for(int col = 0 ; col < set.columnCount() ; col++)
					{
						QVariant v = t->value(set.colHeader(col)->name());
						set.addData(v);
					}
				}
			}

			bool ScoutingDataModel::createCustomDataSet(ScoutingDataSet& set, const QString& query, QString& error)
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

				if (!db->executeQuery(*this, query, set, error))
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

			void ScoutingDataModel::generateRandomScoutingData(GameRandomProfile& profile, int redmax, int bluemax)
			{
				for (auto team : teams_)
				{
					team->setTeamScoutingData(generateRandomData(profile, team_scouting_form_), true);
				}

				for (auto match : matches_)
				{
					if (match->match() <= redmax)
					{
						for (int i = 1; i <= 3; i++)
							match->setScoutingData(Alliance::Red, i, generateRandomData(profile, match_scouting_form_), true);
					}

					if (match->match() <= bluemax)
					{
						for(int i = 1 ; i <= 3 ; i++)
							match->setScoutingData(Alliance::Blue, i, generateRandomData(profile, match_scouting_form_), true);
					}
				}

				emitChangedSignal(ChangeType::PitScoutingDataAdded);
				emitChangedSignal(ChangeType::MatchScoutingDataAdded);
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

			std::vector<std::shared_ptr<FieldDesc>> ScoutingDataModel::getAllFields() const
			{
				std::vector<std::shared_ptr<FieldDesc>> ret;

				ret = getMatchFields();

				auto list = getTeamFields();
				ret.insert(ret.end(), list.begin(), list.end());

				return ret;
			}

			std::vector<std::shared_ptr<FieldDesc>> ScoutingDataModel::getTeamFields() const
			{
				std::vector<std::shared_ptr<FieldDesc>> list;

				for (auto f : team_extra_fields_)
					list.push_back(f);

				auto flist = team_scouting_form_->fields();
				for (auto f : flist)
					list.push_back(f);

				return list;
			}


			std::vector<std::shared_ptr<FieldDesc>> ScoutingDataModel::getMatchFields() const
			{
				std::vector<std::shared_ptr<FieldDesc>> list;

				auto flist = match_scouting_form_->fields();
				for (auto f : flist)
					list.push_back(f);

				for (auto f : match_extra_fields_)
					list.push_back(f);

				return list;
			}

			void ScoutingDataModel::changeTeamData(const QString& tkey, const QString& field, const QVariant& value)
			{
				auto t = findTeamByKeyInt(tkey);
				ScoutingDataMapPtr data = std::make_shared<ScoutingDataMap>();
				*data = *t->teamScoutingData();
				data->insert_or_assign(field, value);
				t->setTeamScoutingData(data, true);

				TabletIdentity id("Central", QUuid());
				QStringList teams;
				QStringList matches;

				teams.push_back(tkey);
				SyncHistoryRecord rec(id, QDateTime::currentDateTime(), teams, matches);
				addHistoryRecord(rec);

				dirty_ = true;
				emitChangedSignal(ChangeType::TeamDataChanged);
			}

			void ScoutingDataModel::changeMatchData(const QString& mkey, const QString& tkey, const QString& field, const QVariant& value)
			{
				auto m = findMatchByKeyInt(mkey);
				Alliance a;
				int slot;

				m->teamToAllianceSlot(tkey, a, slot);

				ScoutingDataMapPtr data = std::make_shared<ScoutingDataMap>();
				*data = *m->scoutingData(a, slot);
				data->insert_or_assign(field, value);
				m->setScoutingData(a, slot, data, true);

				TabletIdentity id("Central", QUuid());
				QStringList teams;
				QStringList matches;

				matches.push_back(mkey);
				SyncHistoryRecord rec(id, QDateTime::currentDateTime(), teams, matches);
				addHistoryRecord(rec);

				dirty_ = true;
				emitChangedSignal(ChangeType::TeamDataChanged);
			}
		}
	}
}
