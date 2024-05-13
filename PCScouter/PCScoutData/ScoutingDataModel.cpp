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
				dirty_ = false;

				emit_changes_ = true;
				invert_zebra_data_ = false;

				addTemporaryFieldDesc();
			}

			ScoutingDataModel::ScoutingDataModel()
			{
				dirty_ = false;
				emit_changes_ = true;
				invert_zebra_data_ = false;

				addTemporaryFieldDesc();
			}

			ScoutingDataModel::~ScoutingDataModel()
			{
			}

			void ScoutingDataModel::getScoutingFormImages(QStringList& imlist)
			{
				if (match_scouting_form_ != nullptr) {
					match_scouting_form_->getImages(imlist);
				}

				if (team_scouting_form_ != nullptr) {
					team_scouting_form_->getImages(imlist);
				}
			}

			void ScoutingDataModel::addTeamExtraData(const QString &tkey, const QString& name, const QVariant& value)
			{
				if (getFieldByName(name) == nullptr)
				{
					if (value.type() == QVariant::Int)
					{
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(name, FieldDesc::Type::Integer, false));
					}
					else if (value.type() == QVariant::Bool)
					{
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(name, FieldDesc::Type::Boolean, false));
					}
					else if (value.type() == QVariant::Double)
					{
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(name, FieldDesc::Type::Double, false));
					}
					else if (value.type() == QVariant::String)
					{
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(name, FieldDesc::Type::String, false));
					}
				}

				auto team = findTeamByKeyInt(tkey);
				team->addExtraData(name, value);
			}

			void ScoutingDataModel::addMatchExtraDataFields(ScoutingDataMapPtr data)
			{
				std::shared_ptr<FieldDesc> desc;
				std::map<QString, std::shared_ptr<FieldDesc>> fields;
				std::map<QString, QStringList> strings;

				for (auto d : *data) {
					auto it = fields.find(d.first);
					if (it != fields.end())
						continue;

					if (d.second.type() == QVariant::Int)
					{
						if (desc == nullptr)
						{
							fields.insert_or_assign(d.first, std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Integer, false));
						}
						else
						{
							assert(desc->type() == FieldDesc::Type::Integer);
						}
					}
					else if (d.second.type() == QVariant::Bool)
					{
						if (desc == nullptr)
						{
							fields.insert_or_assign(d.first, std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Boolean, false));
						}
						else
						{
							assert(desc->type() == FieldDesc::Type::Boolean);
						}
					}
					else if (d.second.type() == QVariant::Double)
					{
						if (desc == nullptr)
						{
							fields.insert_or_assign(d.first, std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Double, false));
						}
						else
						{
							assert(desc->type() == FieldDesc::Type::Double);
						}
					}
					else if (d.second.type() == QVariant::String)
					{
						QStringList list;
						auto it = strings.find(d.first);
						if (it != strings.end())
							list = it->second;

						if (!list.contains(d.second.toString()))
							list.push_back(d.second.toString());

						strings.insert_or_assign(d.first, list);
					}
				}

				for (auto pair : strings)
				{
					if (pair.second.count() > 6) {
						// 
						// Treat as a normal string
						//
						std::shared_ptr<FieldDesc> desc = std::make_shared<FieldDesc>(pair.first, FieldDesc::Type::String, false);
						fields.insert_or_assign(pair.first, desc);
					}
					else
					{
						//
						// Treat as a choice
						//
						std::shared_ptr<FieldDesc> desc = std::make_shared<FieldDesc>(pair.first, pair.second, false);
						fields.insert_or_assign(pair.first, desc);
					}
				}

				std::list<std::shared_ptr<FieldDesc>> descs;
				for (auto pair : fields)
				{
					descs.push_back(pair.second);
				}

				addMatchExtraFields(descs);
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

				if (match_scouting_form_ != nullptr && team_scouting_form_ != nullptr) {
					for (auto field : team_scouting_form_->fields())
					{
						if (match_scouting_form_->itemByName(field->name()) != nullptr)
						{
							dups.push_back(field->name());
							ret = false;
						}
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
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamNickName, FieldDesc::Type::String, false, true));
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamCityName, FieldDesc::Type::String, false, true));
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamStateName, FieldDesc::Type::String, false, true));
				team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::TeamCountryName, FieldDesc::Type::String, false, true));
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
					doc = QJsonDocument::fromJson(data);
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
					doc = QJsonDocument::fromJson(data);
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

				emitChangedSignal(ChangeType::TeamScoutingTabletChanged);
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

			void ScoutingDataModel::addRulesToDataSet(ScoutingDataSet& set)
			{
				auto it = rules_.find(set.name());
				if (it != rules_.end())
				{
					for (auto rule : it->second)
					{
						set.addRule(rule);
					}
				}
			}

			void ScoutingDataModel::createMatchDataSet(ScoutingDataSet& set)
			{
				if (match_scouting_form_ != nullptr) {
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

					addRulesToDataSet(set);
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

				addRulesToDataSet(set);
			}

			bool ScoutingDataModel::createCustomDataSet(ScoutingDataSet& set, const QString& query, QString& error)
			{
				ScoutingDatabase* db = ScoutingDatabase::getInstance();
				if (db == nullptr)
					return false;

				if (!db->hasTable("matches"))
				{
					ScoutingDataSet matchset("$temporary-matches");
					createMatchDataSet(matchset);
					if (!db->addTable("matches", matchset))
						return false;
				}

				if (!db->hasTable("teams"))
				{
					ScoutingDataSet pitset("$temporary-teams");
					createTeamDataSet(pitset);
					if (!db->addTable("teams", pitset))
						return false;
				}

				if (!db->executeQuery(*this, query, set, error))
					return false;

				return true;
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
				auto md = m->scoutingData(a, slot);
				if (md != nullptr) {
					*data = *md;
				}
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
