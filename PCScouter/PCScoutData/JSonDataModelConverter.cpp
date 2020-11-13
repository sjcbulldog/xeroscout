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

#include "JSonDataModelConverter.h"
#include "ScoutDataJsonNames.h"
#include "ScoutingDataModel.h"
#include "CircleFieldRegion.h"
#include "PolygonFieldRegion.h"
#include "RectFieldRegion.h"
#include "SequenceEnterExitPattern.h"
#include "SequenceEnterIdlePattern.h"
#include <QJsonArray>
#include <QTextCodec>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			JSonDataModelConverter::JSonDataModelConverter(ScoutingDataModel* dm)
			{
				dm_ = dm;
				scouting_full_version_ = 1;
				scouting_version_ = 1;
				history_version_ = 1;
				core_version_ = 1;
			}

			JSonDataModelConverter::~JSonDataModelConverter()
			{
			}

			bool JSonDataModelConverter::extractHistory(const QJsonObject& obj)
			{
				bool ret = true;

				if (!obj.contains(JsonVersionName))
				{
					errors_.push_back("JSON object is missing the '" + QString(JsonVersionName) + "' field");
					return false;
				}

				if (!obj.value(JsonVersionName).isDouble())
				{
					errors_.push_back("JSON object has the '" + QString(JsonVersionName) + "' field, but it is not a JSON object");
					return false;
				}

				int version = obj.value(JsonVersionName).toInt();

				if (version == 1)
				{
					ret = extractHistoryV1(obj);
				}
				else
				{
					errors_.push_back("JSON object '" + QString(JsonHistoryName) + "' has an invalid '" + QString(JsonVersionName) + "' field");
					return false;
				}

				return ret;
			}
			bool JSonDataModelConverter::extractHistoryV1(const QJsonObject& obj)
			{
				if (!obj.contains(JsonHistoryDataName))
				{
					errors_.push_back("JSON object is missing the '" + QString(JsonHistoryDataName) + "' field");
					return false;
				}

				if (!obj.value(JsonHistoryDataName).isArray())
				{
					errors_.push_back("JSON object has the '" + QString(JsonHistoryDataName) + "' field, but it is not a JSON array");
					return false;
				}

				QJsonArray arr = obj.value(JsonHistoryDataName).toArray();
				for (int i = 0; i < arr.size(); i++)
				{
					if (!arr[i].isObject())
					{
						errors_.push_back("JSON history array has invalid member, not an object");
						return false;
					}

					QJsonObject rec = arr[i].toObject();

					if (!rec.contains(JsonTabletIdentityName))
					{
						errors_.push_back("JSON object is missing the '" + QString(JsonTabletIdentityName) + "' field");
						return false;
					}

					if (!rec.value(JsonTabletIdentityName).isObject())
					{
						errors_.push_back("JSON object has the '" + QString(JsonTabletIdentityName) + "' field, but it is not a JSON object");
						return false;
					}

					QJsonObject idobj = rec.value(JsonTabletIdentityName).toObject();
					if (!idobj.contains(JsonNameName) || !idobj.value(JsonNameName).isString()) {
						errors_.push_back("expected object in field '" + QString(JsonRegisteredTabletListName) + "' to contain field '" + QString(JsonNameName) + "'");
						return false;
					}

					if (!idobj.contains(JsonUuidName) || !idobj.value(JsonUuidName).isString()) {
						errors_.push_back("expected object in field '" + QString(JsonRegisteredTabletListName) + "' to contain field '" + QString(JsonUuidName) + "'");
						return false;
					}

					QUuid uid = QUuid::fromString(idobj.value(JsonUuidName).toString());
					TabletIdentity id(idobj.value(JsonNameName).toString(), uid);

					if (!rec.contains(JsonHistoryWhenName))
					{
						errors_.push_back("JSON object is missing the '" + QString(JsonHistoryWhenName) + "' field");
						return false;
					}

					if (!rec.value(JsonHistoryWhenName).isDouble())
					{
						errors_.push_back("JSON object has the '" + QString(JsonHistoryWhenName) + "' field, but it is not a JSON object");
						return false;
					}

					QDateTime when = QDateTime::fromMSecsSinceEpoch(rec.value(JsonHistoryWhenName).toVariant().toLongLong());

					if (!rec.contains(JsonHistoryPitListName))
					{
						errors_.push_back("JSON object is missing the '" + QString(JsonHistoryPitListName) + "' field");
						return false;
					}

					if (!rec.value(JsonHistoryPitListName).isArray())
					{
						errors_.push_back("JSON object has the '" + QString(JsonHistoryPitListName) + "' field, but it is not a JSON object");
						return false;
					}

					QStringList pits;
					QJsonArray pitsarr = rec.value(JsonHistoryPitListName).toArray();
					for (int i = 0; i < pitsarr.size(); i++)
					{
						if (pitsarr[i].isString())
							pits.push_back(pitsarr[i].toString());
					}

					if (!rec.contains(JsonHistoryMatchListName))
					{
						errors_.push_back("JSON object is missing the '" + QString(JsonHistoryMatchListName) + "' field");
						return false;
					}

					if (!rec.value(JsonHistoryMatchListName).isArray())
					{
						errors_.push_back("JSON object has the '" + QString(JsonHistoryMatchListName) + "' field, but it is not a JSON object");
						return false;
					}

					QStringList matches;
					QJsonArray matcharr = rec.value(JsonHistoryMatchListName).toArray();
					for (int i = 0; i < matcharr.size(); i++)
					{
						if (matcharr[i].isString())
							matches.push_back(matcharr[i].toString());
					}

					SyncHistoryRecord hrec(id, when, pits, matches);
					dm_->addHistoryRecord(hrec);
				}

				return true;
			}

			QJsonObject JSonDataModelConverter::historyToJson()
			{
				QJsonObject obj;

				QJsonArray a;

				for (const SyncHistoryRecord& rec : dm_->history_)
				{
					QJsonObject obj;
					QJsonObject id;

					id[JsonNameName] = rec.id().name();
					id[JsonUuidName] = rec.id().uid().toString();

					obj[JsonTabletIdentityName] = id;
					obj[JsonHistoryWhenName] = rec.when().toMSecsSinceEpoch();
					obj[JsonHistoryPitListName] = QJsonArray::fromStringList(rec.pits());
					obj[JsonHistoryMatchListName] = QJsonArray::fromStringList(rec.matches());

					a.push_back(obj);
				}

				obj[JsonVersionName] = history_version_;
				obj[JsonHistoryDataName] = a;

				return obj;
			}

			QJsonObject JSonDataModelConverter::coreToJson(bool extra)
			{
				QJsonObject payload;

				payload[JsonVersionName] = core_version_;
				payload[JsonEvKeyName] = dm_->evkey();
				payload[JsonDataModelUUID] = dm_->uuid().toString();
				payload[JsonEvNameName] = dm_->evname();
				payload[JsonMatchScoutingFormName] = dm_->matchScoutingForm()->obj();
				payload[JsonPitScoutingFormName] = dm_->teamScoutingForm()->obj();
				payload[JsonStartDateName] = QString::number(dm_->startDate().toJulianDay());
				payload[JsonEndDateName] = QString::number(dm_->startDate().toJulianDay());

				payload[JsonOriginalPickListDataName] = encodePickList(dm_->original_picklist_);
				payload[JsonPickListDataName] = encodePickList(dm_->picklist_);
				payload[JsonRobotCapablitiesDataName] = encodeRobotCapablities(dm_->robot_capablities_);

				payload[JsonTeamExtraFieldsName] = encodeFields(dm_->teamExtraFields());
				payload[JsonMatchExtraFieldsName] = encodeFields(dm_->matchExtraFields());

				QJsonArray regtabs;
				for (const TabletIdentity& id : dm_->registeredTablets()) {
					QJsonObject obj;
					obj[JsonNameName] = id.name();
					obj[JsonUuidName] = id.uid().toString();
				}
				payload[JsonRegisteredTabletListName] = regtabs;

				QJsonArray teams;
				for (auto team : dm_->teams()) {
					QJsonObject obj;
					obj[JsonKeyName] = team->key();
					obj[JsonNumberName] = team->number();
					obj[JsonNickName] = team->nick();
					obj[JsonNameName] = team->name();
					obj[JsonCityName] = team->city();
					obj[JsonStateName] = team->state();
					obj[JsonCountryName] = team->country();
					obj[JsonAssignName] = team->tablet();
					obj[JsonExtraTeamData] = encode(team->extraData());

					if (team->hasRankingInfo())
						obj[JsonRankingBlobName] = team->rankingInfo();

					teams.push_back(obj);
				}
				payload[JsonTeamsName] = teams;

				QJsonArray matches;
				for (auto m : dm_->matches()) {
					QJsonObject obj;

					obj[JsonKeyName] = m->key();
					obj[JsonCompTypeName] = m->compType();
					obj[JsonSetName] = m->set();
					obj[JsonMatchName] = m->match();
					obj[JsonEtimeName] = m->eventTime();

					QJsonArray red;
					for (int i = 1; i <= 3; i++)
					{
						QString t = m->team(Alliance::Red, i);
						QString assign = m->tablet(Alliance::Red, i);

						red.push_back(t);
						red.push_back(assign);
					}
					obj[JsonRedName] = red;

					QJsonArray blue;
					for (int i = 1; i <= 3 ; i++)
					{
						QString t = m->team(Alliance::Blue, i);
						QString assign = m->tablet(Alliance::Blue, i);

						blue.push_back(t);
						blue.push_back(assign);
					}
					obj[JsonBlueName] = blue;

					QJsonArray extrared;
					QJsonArray extrablue;

					if (extra)
					{
						for (int i = 1; i <= 3; i++) {
							extrared.push_back(encode(m->extraData(Alliance::Red, i)));
							extrablue.push_back(encode(m->extraData(Alliance::Blue, i)));
						}
					}

					obj[JsonExtraDataRedName] = extrared;
					obj[JsonExtraDataBlueName] = extrablue;

					if (extra)
						obj[JsonZebraDataName] = m->zebra();

					if (extra && m->videos_.size() > 0)
					{
						QJsonArray videos;

						for (auto pair : m->videos_)
						{
							QJsonObject vobj;
							vobj[JsonTypeName] = pair.first;
							vobj[JsonKeyName] = pair.second;
							videos.push_back(vobj);
						}

						obj[JsonVideosDataName] = videos;
					}

					matches.push_back(obj);
				}
				payload[JsonMatchesName] = matches;

				QJsonArray match_tablets;
				for (auto mt : dm_->matchTablets())
					match_tablets.push_back(mt);
				payload[JsonMatchTabletListName] = match_tablets;

				QJsonArray team_tablets;
				for (auto t : dm_->teamTablets())
					team_tablets.push_back(t);
				payload[JsonTeamTabletListName] = team_tablets;

				return payload;
			}

			QJsonArray JSonDataModelConverter::encodeScoutingData(const std::list<ConstScoutingDataMapPtr>& data)
			{
				QJsonArray arr;

				for (auto d : data)
					arr.push_back(encode(d));

				return arr;
			}

			QJsonObject JSonDataModelConverter::scoutingToJsonFull()
			{
				QJsonObject obj;
				QJsonArray pits, matches;

				for (auto t : dm_->teams()) 
				{
					QJsonObject tm;
					QJsonArray arr;

					tm[JsonKeyName] = t->key();
					for (auto sd : t->teamScoutingDataList())
						arr.push_back(encode(sd));
					tm[JsonPitsName] = arr;

					pits.push_back(tm);
				}
				
				for (auto m : dm_->matches())
				{
					QJsonObject mt;
					QJsonArray arr;

					mt[JsonKeyName] = m->key();

					Alliance c = Alliance::Red;
					for (int i = 1; i <= 3; i++)
					{
						QString key = toString(c) + QString::number(i);
						mt[key] = encodeScoutingData(m->scoutingDataList(c, i));
					}

					c = Alliance::Blue;
					for (int i = 1; i <= 3; i++)
					{
						QString key = toString(c) + QString::number(i);
						mt[key] = encodeScoutingData(m->scoutingDataList(c, i));
					}

					matches.push_back(mt);
				}

				obj[JsonVersionName] = scouting_full_version_;
				obj[JsonPitsName] = pits;
				obj[JsonMatchesName] = matches;

				return obj;
			}

			QJsonObject JSonDataModelConverter::scoutingToJson(const TabletIdentity* sender, const QString& tablet)
			{
				QJsonObject obj;
				QJsonObject idobj;

				if (sender == nullptr)
				{
					idobj[JsonNameName] = tablet;
					idobj[JsonUuidName] = "";
				}
				else
				{
					idobj[JsonNameName] = sender->name();
					idobj[JsonUuidName] = sender->uid().toString();
				}

				obj[JsonTabletIdentityName] = idobj;
				obj[JsonVersionName] = scouting_version_;

				QJsonArray pits;
				for (auto t : dm_->teams()) {
					if (t->teamScoutingData() != nullptr)
					{
						QJsonObject team;
						team[JsonTeamName] = t->key();
						team[JsonResultName] = encode(t->teamScoutingData());

						pits.push_back(team);
					}
				}
				obj[JsonPitsName] = pits;

				QJsonArray matches;
				for (auto m : dm_->matches()) {
					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						if (m->hasScoutingData(c, slot)) {
							QJsonObject match;
							match[JsonKeyName] = m->key();
							match[JsonAllianceName] = toString(c);
							match[JsonSlotName] = slot;
							match[JsonResultName] = encode(m->scoutingData(c, slot));

							matches.push_back(match);
						}
					}

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						if (m->hasScoutingData(c, slot)) {
							QJsonObject match;
							match[JsonKeyName] = m->key();
							match[JsonAllianceName] = toString(c);
							match[JsonSlotName] = slot;
							match[JsonResultName] = encode(m->scoutingData(c, slot));

							matches.push_back(match);
						}
					}
				}

				obj[JsonMatchesName] = matches;
				return obj;
			}

			QJsonDocument JSonDataModelConverter::coreToJsonDoc(bool extra)
			{
				QJsonDocument doc;
				QJsonObject obj;

				obj[JsonTypeName] = JsonCoreName;
				obj[JsonPayloadName] = coreToJson(extra);

				doc.setObject(obj);

				return doc;
			}

			QJsonDocument JSonDataModelConverter::tabletToJsonDoc(const QString& tablet) {
				QJsonDocument doc;
				QJsonObject obj;

				obj[JsonTypeName] = JsonTableDataName;
				obj[JsonCoreName] = coreToJson(false);
				obj[JsonScoutingName] = scoutingToJson(nullptr, tablet);

				doc.setObject(obj);
				return doc;
			}

			QJsonDocument JSonDataModelConverter::scoutingToJsonDoc(const TabletIdentity* sender, const QString& tablet)
			{
				QJsonDocument doc;
				QJsonObject obj;

				obj[JsonTypeName] = JsonScoutingName;
				obj[JsonPayloadName] = scoutingToJson(sender, tablet);
				doc.setObject(obj);

				return doc;
			}

			QJsonDocument JSonDataModelConverter::allToJsonDoc()
			{
				QJsonDocument doc;
				QJsonObject obj;
				QJsonArray scouting;

				obj[JsonTypeName] = JsonAllName;
				obj[JsonCoreName] = coreToJson(true);
				obj[JsonScoutingName] = scoutingToJsonFull();
				obj[JsonHistoryName] = historyToJson();
				if (dm_->pickListTranslator() != nullptr)
					obj[JsonPickListName] = dm_->pickListTranslator()->toJSON();
				obj[JsonGraphViewsName] = dm_->graphDescriptors().generateJSON();
				obj[JsonTeamSummaryFieldsName] = encodeStringList(dm_->teamSummaryFields());
				obj[JsonDatasetColumnOrdersName] = encodeColumnOrders();
				obj[JsonFieldRegionsName] = encodeFieldRegions();
				obj[JsonActivitiesName] = encodeActivities();
				obj[JsonRulesName] = encodeRules();
				doc.setObject(obj);
				return doc;
			}

			bool JSonDataModelConverter::coreFromJson(const QJsonObject& obj)
			{
				bool ret = true;

				if (!obj.contains(JsonVersionName))
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is missing");
					return false;
				}

				if (!obj.value(JsonVersionName).isDouble())
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is not a number");
					return false;
				}

				int version = obj.value(JsonVersionName).toInt();

				switch (version)
				{
				case 1:
					ret = coreFromJsonV1(obj);
					break;

				default:
					errors_.push_back("unsupported version of data file");
					ret = false;
				}

				return ret;
			}

			bool JSonDataModelConverter::coreFromJsonV1(const QJsonObject& obj)
			{
				if (!extractNameAndKey(obj, 1))
					return false;

				if (!extractRegisteredTabletList(obj, 1))
					return false;

				if (!extractScoutingForms(obj, 1))
					return false;

				if (!extractTeams(obj, 1))
					return false;

				if (!extractMatches(obj, 1))
					return false;

				if (!extractTabletLists(obj, 1))
					return false;

				if (obj.contains(JsonPickListDataName) && obj.value(JsonPickListDataName).isArray())
					decodePickList(obj.value(JsonPickListDataName).toArray(), dm_->picklist_);

				if (obj.contains(JsonOriginalPickListDataName) && obj.value(JsonOriginalPickListDataName).isArray())
					decodePickList(obj.value(JsonOriginalPickListDataName).toArray(), dm_->original_picklist_);

				if (obj.contains(JsonRobotCapablitiesDataName) && obj.value(JsonRobotCapablitiesDataName).isArray())
					decodeRobotCapablities(obj.value(JsonRobotCapablitiesDataName).toArray(), dm_->robot_capablities_);

				return true;
			}

			bool JSonDataModelConverter::extractNameAndKey(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonEvKeyName) || !obj.value(JsonEvKeyName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvKeyName) + "'");
					return false;
				}

				dm_->ev_key_ = obj[JsonEvKeyName].toString();

				if (!obj.contains(JsonDataModelUUID) || !obj.value(JsonDataModelUUID).isString()) {
					errors_.push_back("expected string field '" + QString(JsonDataModelUUID) + "'");
					return false;
				}
				dm_->uuid_ = QUuid::fromString(obj.value(JsonDataModelUUID).toString());

				if (!obj.contains(JsonEvNameName) || !obj.value(JsonEvNameName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvNameName) + "'");
					return false;
				}
				dm_->event_name_ = obj[JsonEvNameName].toString();

				if (!obj.contains(JsonStartDateName) || !obj.value(JsonStartDateName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvKeyName) + "'");
					return false;
				}

				qint64 julian = obj.value(JsonStartDateName).toString().toLongLong();
				dm_->start_date_ = QDate::fromJulianDay(julian);

				if (!obj.contains(JsonEndDateName) || !obj.value(JsonEndDateName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvKeyName) + "'");
					return false;
				}

				julian = obj.value(JsonEndDateName).toString().toLongLong();
				dm_->end_date_ = QDate::fromJulianDay(julian);

				return true;
			}

			bool JSonDataModelConverter::extractRegisteredTabletList(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonRegisteredTabletListName) || !obj.value(JsonRegisteredTabletListName).isArray()) {
					errors_.push_back("expected array field '" + QString(JsonRegisteredTabletListName) + "'");
					return false;
				}

				QJsonArray regarr = obj.value(JsonRegisteredTabletListName).toArray();
				for (int i = 0; i < regarr.size(); i++)
				{
					if (!regarr[i].isObject()) {
						errors_.push_back("expected field '" + QString(JsonRegisteredTabletListName) + "' to be an array of object");
						return false;
					}

					QJsonObject regtab = regarr[i].toObject();

					if (!regtab.contains(JsonNameName) || !regtab.value(JsonNameName).isString()) {
						errors_.push_back("expected object in field '" + QString(JsonRegisteredTabletListName) + "' to contain field '" + QString(JsonNameName) + "'");
						return false;
					}

					if (!regtab.contains(JsonUuidName) || !regtab.value(JsonUuidName).isString()) {
						errors_.push_back("expected object in field '" + QString(JsonRegisteredTabletListName) + "' to contain field '" + QString(JsonUuidName) + "'");
						return false;
					}

					QUuid uid = QUuid::fromString(regtab.value(JsonUuidName).toString());
					if (uid.isNull())
					{
						errors_.push_back("expected object in field '" + QString(JsonRegisteredTabletListName) + "' to contain field '" + QString(JsonUuidName) + "' which is a valid UUID");
						return false;
					}
					TabletIdentity id(regtab.value(JsonNameName).toString(), uid);
					dm_->registered_tablets_.push_back(id);
				}

				return true;
			}

			bool JSonDataModelConverter::extractScoutingForms(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonMatchScoutingFormName) || !obj.value(JsonMatchScoutingFormName).isObject()) {
					errors_.push_back("expected object field 'match_scouting_form'");
					return false;
				}

				if (!obj.contains(JsonPitScoutingFormName) || !obj.value(JsonPitScoutingFormName).isObject()) {
					errors_.push_back("expected object field 'pit_scouting_form'");
					return false;
				}

				dm_->match_scouting_form_ = std::make_shared<ScoutingForm>(obj[JsonMatchScoutingFormName].toObject());
				if (!dm_->match_scouting_form_->isOK()) {
					for (const QString& err : dm_->match_scouting_form_->errors())
						errors_.push_back(err);
					return false;
				}

				dm_->team_scouting_form_ = std::make_shared<ScoutingForm>(obj[JsonPitScoutingFormName].toObject());
				if (!dm_->team_scouting_form_->isOK()) {
					for (const QString& err : dm_->match_scouting_form_->errors())
						errors_.push_back(err);
					return false;
				}

				auto fields = decodeFields(obj.value(JsonTeamExtraFieldsName).toArray());
				dm_->addTeamExtraFields(fields);

				fields = decodeFields(obj.value(JsonMatchExtraFieldsName).toArray());
				dm_->addMatchExtraFields(fields);

				return true;
			}

			bool JSonDataModelConverter::extractTeams(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonTeamsName) || !obj.value(JsonTeamsName).isArray()) {
					errors_.push_back("expected object field 'teams'");
					return false;
				}
				QJsonArray teams = obj[JsonTeamsName].toArray();

				for (auto team : teams) {
					if (!team.isObject()) {
						errors_.push_back("expected 'teams' field to be an array of objects");
						return false;
					}

					QJsonObject tobj = team.toObject();

					if (!tobj.contains(JsonKeyName) || !tobj.value(JsonKeyName).isString()) {
						errors_.push_back("expected string field 'key'");
						return false;
					}

					if (!tobj.contains(JsonNumberName) || !tobj.value(JsonNumberName).isDouble()) {
						errors_.push_back("expected string field 'number'");
						return false;
					}

					if (!tobj.contains(JsonNameName) || !tobj.value(JsonNameName).isString()) {
						errors_.push_back("expected string field 'name'");
						return false;
					}

					if (!tobj.contains(JsonAssignName) || !tobj.value(JsonAssignName).isString()) {
						errors_.push_back("expected string field 'assign'");
						return false;
					}

					if (!tobj.contains(JsonExtraTeamData) || !tobj.value(JsonExtraTeamData).isArray()) {
						errors_.push_back("expected array field 'extra'");
						return false;
					}

					QString key = tobj[JsonKeyName].toString();
					int number = tobj[JsonNumberName].toInt();
					QString name = tobj[JsonNameName].toString();
					QString nick = tobj[JsonNickName].toString();
					QString city = tobj[JsonCityName].toString();
					QString state = tobj[JsonStateName].toString();
					QString country = tobj[JsonCountryName].toString();

					auto t = dm_->addTeam(key, number, nick, name, city, state, country);
					dm_->assignTeamTablet(key, tobj[JsonAssignName].toString());
					t->addExtraData(decode(tobj[JsonExtraTeamData].toArray()));

					if (tobj.contains(JsonRankingBlobName) && tobj.value(JsonRankingBlobName).isObject())
					{
						dm_->setTeamRanking(key, tobj.value(JsonRankingBlobName).toObject());
					}

					
				}

				return true;
			}

			bool JSonDataModelConverter::extractMatches(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonMatchesName) || !obj.value(JsonMatchesName).isArray()) {
					errors_.push_back("expected object field 'matches'");
					return false;
				}
				QJsonArray matches = obj[JsonMatchesName].toArray();

				for (auto m : matches)
				{
					if (!m.isObject())
					{
						errors_.push_back("the 'matches' field is expected to be an array of objects");
						return false;
					}
					QJsonObject mobj = m.toObject();

					if (!mobj.contains(JsonKeyName) || !mobj.value(JsonKeyName).isString()) {
						errors_.push_back("expected string field 'key'");
						return false;
					}

					if (!mobj.contains(JsonCompTypeName) || !mobj.value(JsonCompTypeName).isString()) {
						errors_.push_back("expected string field 'comp_type'");
						return false;
					}

					if (!mobj.contains(JsonSetName) || !mobj.value(JsonSetName).isDouble()) {
						errors_.push_back("expected string field 'set'");
						return false;
					}

					if (!mobj.contains(JsonMatchName) || !mobj.value(JsonMatchName).isDouble()) {
						errors_.push_back("expected string field 'match'");
						return false;
					}

					if (!mobj.contains(JsonEtimeName) || !mobj.value(JsonEtimeName).isDouble()) {
						errors_.push_back("expected string field 'match'");
						return false;
					}

					if (!mobj.contains(JsonRedName) || !mobj.value(JsonRedName).isArray()) {
						errors_.push_back("expected string field 'red'");
						return false;
					}

					if (!mobj.contains(JsonBlueName) || !mobj.value(JsonBlueName).isArray()) {
						errors_.push_back("expected string field 'blue'");
						return false;
					}

					if (!mobj.contains(JsonExtraDataRedName) || !mobj.value(JsonExtraDataRedName).isArray()) {
						errors_.push_back("expected JSON array field 'badata'");
						return false;
					}

					if (!mobj.contains(JsonExtraDataBlueName) || !mobj.value(JsonExtraDataBlueName).isArray()) {
						errors_.push_back("expected JSON array field 'badata'");
						return false;
					}

					auto dm = dm_->addMatch(mobj.value(JsonKeyName).toString(), mobj.value(JsonCompTypeName).toString(), mobj.value(JsonSetName).toInt(), mobj.value(JsonMatchName).toInt(), mobj.value(JsonEtimeName).toInt());

					QJsonArray red = mobj.value(JsonRedName).toArray();
					for (int i = 0; i < red.size(); i += 2) {
						int slot = i / 2 + 1;
						if (!red[i].isString() || !red[i + 1].isString())
						{
							errors_.push_back("expected field 'red' to be an array of strings (teamkeys and tablets)");
							return false;
						}

						auto t = dm_->findTeamByKey(red[i].toString());
						dm->addTeam(Alliance::Red, slot, red[i].toString(), t->number());
						dm->setTablet(Alliance::Red, slot, red[i + 1].toString());
					}

					QJsonArray blue = mobj.value(JsonBlueName).toArray();
					for (int i = 0; i < blue.size(); i += 2) {
						int slot = i / 2 + 1;
						if (!red[i].isString() || !red[i + 1].isString())
						{
							errors_.push_back("expected field 'red' to be an array of strings (teamkeys and tablets)");
							return false;
						}

						auto t = dm_->findTeamByKey(red[i].toString());
						dm->addTeam(Alliance::Blue, slot, blue[i].toString(), t->number());
						dm->setTablet(Alliance::Blue, slot, blue[i + 1].toString());
					}

					QJsonArray badatared = mobj.value(JsonExtraDataRedName).toArray();
					QJsonArray badatablue = mobj.value(JsonExtraDataBlueName).toArray();

					if (badatablue.size() == 3 && badatared.size() == 3) {
						for (int i = 1; i <= 3; i++) {
							dm->addExtraData(Alliance::Red, i, decode(badatared[i - 1].toArray()));
							dm->addExtraData(Alliance::Blue, i, decode(badatablue[i - 1].toArray()));
						}
					}

					if (mobj.contains(JsonZebraDataName) && mobj.value(JsonZebraDataName).isObject())
						dm->setZebra(mobj.value(JsonZebraDataName).toObject());

					if (mobj.contains(JsonVideosDataName) && mobj.value(JsonVideosDataName).isArray())
					{
						QJsonArray array = mobj.value(JsonVideosDataName).toArray();
						for (int i = 0; i < array.size(); i++)
						{
							if (!array[i].isObject())
								continue;

							QJsonObject vobj = array[i].toObject();

							if (!vobj.contains(JsonTypeName) || !vobj.value(JsonTypeName).isString())
								continue;

							if (!vobj.contains(JsonKeyName) || !vobj.value(JsonKeyName).isString())
								continue;

							dm->videos_.push_back(std::make_pair(vobj.value(JsonTypeName).toString(), vobj.value(JsonKeyName).toString()));
						}
					}
				}

				return true;
			}

			bool JSonDataModelConverter::extractTabletLists(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonMatchTabletListName) || !obj.value(JsonMatchTabletListName).isArray()) {
					errors_.push_back("expected array field 'matchtablets'");
					return false;
				}

				QJsonArray a = obj[JsonMatchTabletListName].toArray();
				for (int i = 0; i < a.size(); i++) {
					if (a[i].isString())
						dm_->match_tablets_.push_back(a[i].toString());
				}

				if (!obj.contains(JsonTeamTabletListName) || !obj.value(JsonTeamTabletListName).isArray()) {
					errors_.push_back("expected array field 'pittablets'");
					return false;
				}

				a = obj[JsonTeamTabletListName].toArray();
				for (int i = 0; i < a.size(); i++) {
					if (a[i].isString())
						dm_->team_tablets_.push_back(a[i].toString());
				}

				return true;
			}

			bool JSonDataModelConverter::scoutingFromJsonFullV1Matches(const QJsonArray& arr)
			{
				for (int i = 0; i < arr.size(); i++)
				{
					if (!arr[i].isObject())
					{
						errors_.push_back("expected object in match scouting array");
						return false;
					}

					QJsonObject obj = arr[i].toObject();

					if (!obj.contains(JsonKeyName) || !obj.value(JsonKeyName).isString())
					{
						errors_.push_back("expected match key field in match scouting data");
						return false;
					}
					QString matchkey = obj.value(JsonKeyName).toString();
					QStringList keys = obj.keys();

					Alliance c = Alliance::Red;
					for (int i = 1; i <= 3; i++)
					{
						QString key = toString(c) + QString::number(i);
						if (!obj.contains(key) || !obj.value(key).isArray())
						{
							errors_.push_back("illegal match scouting data");
							return false;
						}

						QJsonArray dataarray = obj.value(key).toArray();
						for (int d = 0; d < dataarray.size(); d++)
						{
							if (!dataarray[d].isArray())
							{
								errors_.push_back("invalid scouting data in JSON file");
								return false;
							}

							ScoutingDataMapPtr dptr = decode(dataarray[d].toArray());
							dm_->setMatchScoutingData(matchkey, c, i, dptr);
						}
					}

					c = Alliance::Blue;
					for (int i = 1; i <= 3; i++)
					{
						QString key = toString(c) + QString::number(i);
						if (!obj.contains(key) || !obj.value(key).isArray())
						{
							errors_.push_back("illegal match scouting data");
							return false;
						}

						QJsonArray dataarray = obj.value(key).toArray();
						for (int d = 0; d < dataarray.size(); d++)
						{
							if (!dataarray[d].isArray())
							{
								errors_.push_back("invalid scouting data in JSON file");
								return false;
							}

							ScoutingDataMapPtr dptr = decode(dataarray[d].toArray());
							dm_->setMatchScoutingData(matchkey, c, i, dptr);
						}
					}
				}

				return true;
			}

			bool JSonDataModelConverter::scoutingFromJsonFullV1Teams(const QJsonArray& arr)
			{
				for (int i = 0; i < arr.size(); i++)
				{
					if (!arr[i].isObject())
					{
						errors_.push_back("expected object in pits scouting array");
						return false;
					}

					QJsonObject obj = arr[i].toObject();

					if (!obj.contains(JsonKeyName) || !obj.value(JsonKeyName).isString())
					{
						errors_.push_back("expected match key field in pits scouting data");
						return false;
					}
					QString teamkey = obj.value(JsonKeyName).toString();

					if (!obj.contains(JsonPitsName) || !obj.value(JsonPitsName).isArray())
					{
						errors_.push_back("expected team scouting data in scouting array");
						return false;
					}

					QJsonArray dataarray = obj.value(JsonPitsName).toArray();
					for (int d = 0; d < dataarray.size(); d++)
					{
						if (!dataarray[d].isArray())
						{
							errors_.push_back("invalid scouting data in JSON file");
							return false;
						}
						ScoutingDataMapPtr dptr = decode(dataarray[d].toArray());
						dm_->setTeamScoutingData(teamkey, dptr);
					}

				}
				return true;
			}

			bool JSonDataModelConverter::scoutingFromJsonFullV1(const QJsonObject& obj)
			{
				if (!obj.contains(JsonMatchesName) || !obj.value(JsonMatchesName).isArray())
				{
					errors_.push_back("expected array field 'matches'");
					return false;
				}

				if (!obj.contains(JsonPitsName) || !obj.value(JsonPitsName).isArray())
				{
					errors_.push_back("expected array field 'matches'");
					return false;
				}

				if (!scoutingFromJsonFullV1Matches(obj.value(JsonMatchesName).toArray()))
					return false ;

				if (!scoutingFromJsonFullV1Teams(obj.value(JsonPitsName).toArray()))
					return false;

				return true;
			}

			bool JSonDataModelConverter::scoutingFromJsonFull(const QJsonObject& obj)
			{
				bool ret = false;

				if (!obj.contains(JsonVersionName))
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is missing");
					return false;
				}

				if (!obj.value(JsonVersionName).isDouble())
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is not a number");
					return false;
				}

				int version = obj.value(JsonVersionName).toInt();

				switch (version)
				{
				case 1:
					ret = scoutingFromJsonFullV1(obj);
					break;

				default:
					errors_.push_back("unsupported version of data file");
					ret = false;
				}

				return ret;
			}

			bool JSonDataModelConverter::scoutingFromJson(const QJsonObject& obj, TabletIdentity& id, QStringList& pits, QStringList& matches)
			{
				QString tablet;
				bool ret = true;

				if (!obj.contains(JsonVersionName))
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is missing");
					return false;
				}

				if (!obj.value(JsonVersionName).isDouble())
				{
					errors_.push_back("field '" + QString(JsonVersionName) + "' is not a number");
					return false;
				}

				int version = obj.value(JsonVersionName).toInt();

				switch (version)
				{
				case 1:
					ret = scoutingFromJsonV1(obj, id, pits, matches);
					break;

				default:
					errors_.push_back("unsupported version of data file");
					ret = false;
				}

				return ret;
			}

			bool JSonDataModelConverter::scoutingFromJsonV1(const QJsonObject& obj, TabletIdentity& id, QStringList& pits, QStringList& matches)
			{
				if (!extractTabletID(obj, 1, id))
					return false;

				if (!extractPits(obj, 1, id.name(), pits))
					return false;

				if (!extractMatches(obj, 1, id.name(), matches))
					return false;

				return true;
			}

			bool JSonDataModelConverter::extractTabletID(const QJsonObject& obj, int version, TabletIdentity& tablet)
			{
				if (!obj.contains(JsonTabletIdentityName) || !obj.value(JsonTabletIdentityName).isObject()) {
					errors_.push_back("scouting data was missing the object field '" + QString(JsonTabletIdentityName) + "'");
					return false;
				}

				QJsonObject idobj = obj.value(JsonTabletIdentityName).toObject();

				if (!idobj.contains(JsonNameName) || !idobj.value(JsonNameName).isString()) {
					errors_.push_back("scouting data ID field was missing the string field 'name'");
					return false;
				}

				if (!idobj.contains(JsonUuidName) || !idobj.value(JsonUuidName).isString()) {
					errors_.push_back("scouting data ID field was missing the string field 'uuid'");
					return false;
				}

				QString uid = idobj.value(JsonUuidName).toString();
				QString name = idobj.value(JsonNameName).toString();

				if (uid.length() == 0)
					tablet = TabletIdentity(name);
				else
					tablet = TabletIdentity(name, QUuid::fromString(uid));

				return true;
			}

			bool JSonDataModelConverter::extractPits(const QJsonObject& obj, int version, const QString& tablet, QStringList& added)
			{
				if (!obj.contains(JsonPitsName) || !obj.value(JsonPitsName).isArray()) {
					errors_.push_back("scouting data was missing the array field 'pits'");
					return false;
				}

				QJsonArray pits = obj.value(JsonPitsName).toArray();

				for (int i = 0; i < pits.size(); i++) {
					if (!pits[i].isObject())
					{
						errors_.push_back("scouting data for pits was corrupt, expected JSON objects in array");
						return false;
					}

					QJsonObject pitres = pits[i].toObject();

					if (!pitres.contains(JsonTeamName) || !pitres.value(JsonTeamName).isString())
					{
						errors_.push_back("scouting data for pits was corrupt, expected string field 'team' in team scouting result");
						return false;
					}

					if (!pitres.contains(JsonResultName) || !pitres.value(JsonResultName).isArray())
					{
						errors_.push_back("scouting data for pits was corrupt, expected array field 'result' in team scouting result");
						return false;
					}

					if (dm_->setTeamScoutingData(pitres.value(JsonTeamName).toString(), decode(pitres.value(JsonResultName).toArray())))
						added.push_back(pitres.value(JsonTeamName).toString());
				}

				return true;
			}

			bool JSonDataModelConverter::extractMatches(const QJsonObject& obj, int version, const QString& tablet, QStringList& added)
			{
				if (!obj.contains(JsonMatchesName) || !obj.value(JsonMatchesName).isArray()) {
					errors_.push_back("scouting data was missing the array field 'matches'");
					return false;
				}

				QJsonArray matches = obj.value(JsonMatchesName).toArray();

				for (int i = 0; i < matches.size(); i++)
				{
					if (!matches[i].isObject())
					{
						errors_.push_back("scouting data for matches was corrupt, expected JSON objects in array");
						return false;
					}

					QJsonObject matchres = matches[i].toObject();

					if (!matchres.contains(JsonKeyName) || !matchres.value(JsonKeyName).isString())
					{
						errors_.push_back("scouting data for pits was corrupt, expected string field 'key' in team scouting result");
						return false;
					}

					if (!matchres.contains("alliance") || !matchres.value("alliance").isString())
					{
						errors_.push_back("scouting data for pits was corrupt, expected string field 'alliance' in team scouting result");
						return false;
					}

					if (!matchres.contains("slot") || !matchres.value("slot").isDouble())
					{
						errors_.push_back("scouting data for pits was corrupt, expected integer field 'slot' in team scouting result");
						return false;
					}

					if (!matchres.contains(JsonResultName) || !matchres.value(JsonResultName).isArray())
					{
						errors_.push_back("scouting data for pits was corrupt, expected JSON array field 'result' in team scouting result");
						return false;
					}

					auto m = dm_->findMatchByKey(matchres.value(JsonKeyName).toString());
					Alliance c = allianceFromString(matchres.value(JsonAllianceName).toString());

					if (dm_->setMatchScoutingData(m->key(), c, matchres.value(JsonSlotName).toInt(), decode(matchres.value(JsonResultName).toArray())))
						added.push_back(matchres.value(JsonKeyName).toString());
				}

				return true;
			}

			bool JSonDataModelConverter::peekUUID(const QJsonDocument &doc, QUuid& uuid)
			{
				if (!doc.isObject())
					return false;

				QJsonObject obj = doc.object();

				if (!obj.contains(JsonPayloadName) || !obj.value(JsonPayloadName).isObject())
					return false;

				obj = obj.value(JsonPayloadName).toObject();

				if (!obj.contains(JsonDataModelUUID) || !obj.value(JsonDataModelUUID).isString())
					return false;

				uuid = QUuid::fromString(obj.value(JsonDataModelUUID).toString());
				return true;
			}

			bool JSonDataModelConverter::coreFromJson(const QJsonDocument& doc)
			{
				if (!doc.isObject()) {
					errors_.push_back("JSON document is not an object");
					return false;
				}

				QJsonObject obj = doc.object();
				if (!obj.contains(JsonTypeName) || !obj.value(JsonTypeName).isString())
				{
					errors_.push_back("JSON document missing string 'type' field");
					return false;
				}

				if (obj.value(JsonTypeName).toString() != "core") {
					errors_.push_back("JSON document is not a 'core' document");
					return false;
				}

				if (!obj.contains(JsonPayloadName) || !obj.value(JsonPayloadName).isObject())
				{
					errors_.push_back("JSON document does not contain 'payload' field");
					return false;
				}

				return coreFromJson(obj.value(JsonPayloadName).toObject());
			}

			bool JSonDataModelConverter::scoutingFromJson(const QJsonDocument& doc, TabletIdentity& id, QStringList& pits, QStringList& matches)
			{
				if (!doc.isObject())
					return false;

				QJsonArray a = doc.array();
				QJsonObject obj = doc.object();

				if (!obj.contains(JsonTypeName) || !obj.value(JsonTypeName).isString() || obj.value(JsonTypeName).toString() != JsonScoutingName)
					return false;

				if (!obj.contains(JsonPayloadName) || !obj.value(JsonPayloadName).isObject())
					return false;

				QJsonObject payload = obj.value(JsonPayloadName).toObject();

				return scoutingFromJson(payload, id, pits, matches);
			}

			bool JSonDataModelConverter::scoutingFromAllJson(const QJsonDocument& doc)
			{
				bool isTablet = false;

				if (!doc.isObject()) {
					errors_.push_back("JSON document is not an object");
					return false;
				}

				QJsonObject obj = doc.object();
				if (!obj.contains(JsonTypeName) || !obj.value(JsonTypeName).isString())
				{
					errors_.push_back("JSON document missing string 'type' field");
					return false;
				}

				if (obj.value(JsonTypeName).toString() != JsonAllName && obj.value(JsonTypeName).toString() != JsonTableDataName) {
					errors_.push_back("JSON document is not a 'all' document");
					return false;
				}

				if (!obj.contains(JsonScoutingName) || !obj.value(JsonScoutingName).isObject())
				{
					errors_.push_back("JSON document does not contain 'scouting' field");
					return false;
				}

				if (!scoutingFromJsonFull(obj.value(JsonScoutingName).toObject()))
					return false;

				return true;
			}

			bool JSonDataModelConverter::allFromJson(const QJsonDocument& doc)
			{
				bool isTablet = false;

				if (!doc.isObject()) {
					errors_.push_back("JSON document is not an object");
					return false;
				}

				QJsonObject obj = doc.object();
				if (!obj.contains(JsonTypeName) || !obj.value(JsonTypeName).isString())
				{
					errors_.push_back("JSON document missing string 'type' field");
					return false;
				}

				if (obj.value(JsonTypeName).toString() != JsonAllName && obj.value(JsonTypeName).toString() != JsonTableDataName) {
					errors_.push_back("JSON document is not a 'all' document");
					return false;
				}

				if (obj.value(JsonTypeName).toString() == JsonTableDataName)
					isTablet = true;

				if (!obj.contains(JsonCoreName) || !obj.value(JsonCoreName).isObject())
				{
					errors_.push_back("JSON document does not contain 'core' field");
					return false;
				}

				if (!obj.contains(JsonScoutingName) || !obj.value(JsonScoutingName).isObject())
				{
					errors_.push_back("JSON document does not contain 'scouting' field");
					return false;
				}

				if (!isTablet) {
					if (!obj.contains(JsonHistoryName) || !obj.value(JsonHistoryName).isObject())
					{
						errors_.push_back("JSON document does not contain 'history' field");
						return false;
					}
				}

				if (!coreFromJson(obj.value(JsonCoreName).toObject()))
					return false;

				if (!isTablet) {
					if (!extractHistory(obj.value(JsonHistoryName).toObject()))
						return false;

					if (obj.contains(JsonPickListName) && obj.value(JsonPickListName).isArray())
					{
						std::shared_ptr<PickListTranslator> pl = std::make_shared<PickListTranslator>();
						if (!pl->load(obj.value(JsonPickListName).toArray()))
						{
							errors_.push_back(pl->error());
						}
						else
						{
							dm_->setPickListTranslator(pl);
						}
					}
				}

				if (!scoutingFromJsonFull(obj.value(JsonScoutingName).toObject()))
					return false;

				if (obj.contains(JsonGraphViewsName) && obj.value(JsonGraphViewsName).isArray())
				{
					QString err;
					if (!dm_->setGraphViews(obj.value(JsonGraphViewsName).toArray(), err))
					{
						errors_.push_back(err);
						return false;
					}
				}

				if (obj.contains(JsonTeamSummaryFieldsName) && obj.value(JsonTeamSummaryFieldsName).isArray())
					dm_->setTeamSummaryFields(decodeStringList(obj.value(JsonTeamSummaryFieldsName).toArray()));

				if (obj.contains(JsonDatasetColumnOrdersName) && obj.value(JsonDatasetColumnOrdersName).isArray())
					decodeColumnOrders(obj.value(JsonDatasetColumnOrdersName).toArray());

				if (obj.contains(JsonFieldRegionsName) && obj.value(JsonFieldRegionsName).isArray())
					decodeFieldRegions(obj.value(JsonFieldRegionsName).toArray());

				if (obj.contains(JsonActivitiesName) && obj.value(JsonActivitiesName).isArray())
					decodeActivities(obj.value(JsonActivitiesName).toArray());

				if (obj.contains(JsonRulesName) && obj.value(JsonRulesName).isArray())
					decodeRules(obj.value(JsonRulesName).toArray());


				return true;
			}


			QJsonArray JSonDataModelConverter::encodePickList(const std::vector<PickListEntry> &picklist)
			{
				QJsonArray array;

				for (const PickListEntry& entry : picklist)
				{
					QJsonObject obj;
					obj[JsonTeamName] = entry.team();
					obj[JsonScoreName] = entry.score();

					QJsonArray thirds;
					for (size_t i = 0; i < entry.count(); i++)
					{
						QJsonObject tobj;
						tobj[JsonTeamName] = entry.thirdTeam(i);
						tobj[JsonScoreName] = entry.thirdScore(i);

						thirds.push_back(tobj);
					}

					obj[JsonThirdsName] = thirds;
					array.push_back(obj);
				}

				return array;
			}

			void JSonDataModelConverter::decodePickList(const QJsonArray& array, std::vector<PickListEntry>& picklist)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();

					if (!obj.contains(JsonTeamName) || !obj.value(JsonTeamName).isDouble())
						continue;

					if (!obj.contains(JsonScoreName) || !obj.value(JsonScoreName).isDouble())
						continue;

					if (!obj.contains(JsonThirdsName) || !obj.value(JsonThirdsName).isArray())
						continue;

					PickListEntry entry(obj.value(JsonTeamName).toInt(), obj.value(JsonScoreName).toDouble());
					QJsonArray tarray = obj.value(JsonThirdsName).toArray();

					for (int j = 0; j < tarray.size(); j++)
					{
						if (!tarray[j].isObject())
							continue;

						QJsonObject tobj = tarray[j].toObject();

						if (!tobj.contains(JsonTeamName) || !tobj.value(JsonTeamName).isDouble())
							continue;

						if (!tobj.contains(JsonScoreName) || !tobj.value(JsonScoreName).isDouble())
							continue;

						entry.addThird(tobj.value(JsonTeamName).toInt(), tobj.value(JsonScoreName).toDouble());
					}

					picklist.push_back(entry);
				}
			}

			QJsonArray JSonDataModelConverter::encodeRobotCapablities(const std::vector<RobotCapabilities>& robotlist)
			{
				QJsonArray array;

				for (const RobotCapabilities& robot : robotlist)
				{
					QJsonObject obj;
					QJsonArray dnamearray;
					QJsonArray dvaluearray;

					obj[JsonTeamName] = robot.team();
					for (auto pair : robot.doubleParams())
					{
						dnamearray.push_back(pair.first);
						dvaluearray.push_back(pair.second);
					}
					obj[JsonDoubleParamNameListName] = dnamearray;
					obj[JsonDoubleParamValueListName] = dvaluearray;

					QJsonArray xnamearray;
					QJsonArray xvaluearray;
					for (auto pair : robot.distParams())
					{
						xnamearray.push_back(pair.first);
						xvaluearray.push_back(encodeDist(pair.second));
					}

					obj[JsonDistParamNameListName] = xnamearray;
					obj[JsonDistParamValueListName] = xvaluearray;

					array.push_back(obj);
				}

				return array;
			}

			void JSonDataModelConverter::decodeRobotCapablities(const QJsonArray& array, std::vector<RobotCapabilities>& robotlist)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();
					if (!obj.contains(JsonTeamName) || !obj.value(JsonTeamName).isDouble())
						continue;

					if (!obj.contains(JsonDoubleParamNameListName) || !obj.value(JsonDoubleParamNameListName).isArray())
						continue;

					if (!obj.contains(JsonDoubleParamValueListName) || !obj.value(JsonDoubleParamValueListName).isArray())
						continue;

					if (!obj.contains(JsonDistParamNameListName) || !obj.value(JsonDistParamNameListName).isArray())
						continue;

					if (!obj.contains(JsonDistParamValueListName) || !obj.value(JsonDistParamValueListName).isArray())
						continue;

					QJsonArray dnamearray = obj.value(JsonDoubleParamNameListName).toArray();
					QJsonArray dvaluearray = obj.value(JsonDoubleParamValueListName).toArray();
					QJsonArray xnamearray = obj.value(JsonDistParamNameListName).toArray();
					QJsonArray xvaluearray = obj.value(JsonDistParamValueListName).toArray();

					if (dnamearray.size() != dvaluearray.size())
						continue;

					if (xnamearray.size() != xvaluearray.size())
						continue;

					RobotCapabilities robot(obj.value(JsonTeamName).toInt());

					for (int i = 0; i < dnamearray.size(); i++)
					{
						if (dnamearray[i].isString() && dvaluearray[i].isDouble())
						{
							robot.addDoubleParam(dnamearray[i].toString(), dvaluearray[i].toDouble());
						}
					}

					for (int i = 0; i < xnamearray.size(); i++)
					{
						if (xnamearray[i].isString() && xvaluearray[i].isArray())
						{
							Distribution dist;

							if (decodeDist(xvaluearray[i].toArray(), dist))
							{
								robot.addDistParam(xnamearray[i].toString(), dist);
							}
						}
					}

					dm_->robot_capablities_.push_back(robot);
				}
			}

			QJsonArray JSonDataModelConverter::encodeDist(const Distribution& dist)
			{
				QJsonArray distarray;

				for (auto pair : dist)
				{
					QJsonObject obj;
					obj[JsonDistBucketName] = pair.first;
					obj[JsonValueName] = pair.second;

					distarray.push_back(obj);
				}

				return distarray;
			}

			bool JSonDataModelConverter::decodeDist(const QJsonArray& array, Distribution& dist)
			{
				dist.clear();

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();
					if (!obj.contains(JsonDistBucketName) || !obj.value(JsonDistBucketName).isDouble())
						continue;

					if (!obj.contains(JsonValueName) || !obj.value(JsonValueName).isDouble())
						continue;

					int bucket = obj.value(JsonDistBucketName).toInt();
					double value = obj.value(JsonValueName).toDouble();
					dist.push_back(std::make_pair(bucket, value));
				}

				return true;
			}

			QJsonArray JSonDataModelConverter::encodeRules()
			{
				QJsonArray r;

				for (auto rules : dm_->rules_)
				{
					QJsonObject obj;
					QJsonArray ruleset;

					obj[JsonNameName] = rules.first;
					for (auto rule : rules.second)
					{
						QJsonObject onerule;
						QJsonArray slist;
						QJsonArray hlist;

						onerule[JsonNameName] = rule->name();
						onerule[JsonDescriptionName] = rule->descriptor();
						onerule[JsonForegroundName] = rule->foreground().name();
						onerule[JsonBackgroundName] = rule->background().name();
						onerule[JsonEquationName] = rule->equation();

						for (auto s : rule->fields())
							slist.push_back(s);
						onerule[JsonSelectListName] = slist;

						for (auto h : rule->highlights())
							hlist.push_back(h);
						onerule[JsonHighlightListName] = hlist;

						ruleset.push_back(onerule);
					}

					obj[JsonRulesName] = ruleset;
					r.push_back(obj);
				}

				return r;
			}

			void JSonDataModelConverter::decodeRules(const QJsonArray &array)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();
					if (!obj.contains(JsonNameName) || !obj.value(JsonNameName).isString())
						continue;

					if (!obj.contains(JsonRulesName) || !obj.value(JsonRulesName).isArray())
						continue;

					QJsonArray rarray = obj.value(JsonRulesName).toArray();
					std::list<std::shared_ptr<DataSetHighlightRules>> list;

					for (int i = 0; i < rarray.size(); i++)
					{
						if (!rarray[i].isObject())
							continue;

						QJsonObject robj = rarray[i].toObject();

						if (!robj.contains(JsonNameName) || !robj.value(JsonNameName).isString())
							continue;

						if (!robj.contains(JsonDescriptionName) || !robj.value(JsonDescriptionName).isString())
							continue;

						if (!robj.contains(JsonForegroundName) || !robj.value(JsonForegroundName).isString())
							continue;

						if (!robj.contains(JsonBackgroundName) || !robj.value(JsonBackgroundName).isString())
							continue;

						if (!robj.contains(JsonEquationName) || !robj.value(JsonEquationName).isString())
							continue;

						if (!robj.contains(JsonSelectListName) || !robj.value(JsonSelectListName).isArray())
							continue;

						if (!robj.contains(JsonHighlightListName) || !robj.value(JsonHighlightListName).isArray())
							continue;

						QStringList select, highlight;

						QString name = robj[JsonNameName].toString();
						QString equ = robj[JsonEquationName].toString();
						QString desc = robj[JsonDescriptionName].toString();

						QColor foreground = QColor(robj[JsonForegroundName].toString());
						QColor background = QColor(robj[JsonBackgroundName].toString());

						QJsonArray array = robj.value(JsonSelectListName).toArray();
						for (int i = 0; i < array.size(); i++)
						{
							if (array[i].isString())
								select.push_back(array[i].toString());
						}

						array = robj.value(JsonHighlightListName).toArray();
						for (int i = 0; i < array.size(); i++)
						{
							if (array[i].isString())
								highlight.push_back(array[i].toString());
						}

						auto r = std::make_shared<DataSetHighlightRules>(name, select, highlight, equ, foreground, background, desc);
						list.push_back(r);
					}
					dm_->rules_.insert_or_assign(obj.value(JsonNameName).toString(), list);
				}
			}

			QJsonArray JSonDataModelConverter::encodeActivities()
			{
				QJsonArray result;

				for (auto p : dm_->activities())
				{
					QJsonObject obj;
					QJsonArray sarray;

					obj[JsonNameName] = p->name();

					for (auto seq : p->patterns())
					{
						QJsonObject sobj;
						sobj[JsonMinName] = seq->minCount();
						sobj[JsonMaxName] = seq->maxCount();
						sobj[JsonPerAllianceName] = seq->perAlliance();

						auto entexit = std::dynamic_pointer_cast<const SequenceEnterExitPattern>(seq);
						if (entexit != nullptr)
						{
							sobj[JsonTypeName] = "enterexit";
							sobj[JsonNameName] = entexit->name();
							sarray.push_back(sobj);
							continue;
						}

						auto entidle = std::dynamic_pointer_cast<const SequenceEnterIdlePattern>(seq);
						if (entidle != nullptr)
						{
							sobj[JsonTypeName] = "enteridle";
							sobj[JsonNameName] = entidle->name();
							sobj[JsonIdleValueName] = entidle->duration();
							sarray.push_back(sobj);
							continue;
						}

						assert(false);
					}

					obj[JsonPatternsName] = sarray;
					result.push_back(obj);
				}

				return result;
			}

			void JSonDataModelConverter::decodeActivities(const QJsonArray& array)
			{
				int minv, maxv;
				QString name;
				bool perall;

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();

					if (!obj.contains(JsonNameName) || !obj.value(JsonNameName).isString())
						continue;

					if (!obj.contains(JsonPatternsName) || !obj.value(JsonPatternsName).isArray())
						continue;

					auto act = std::make_shared<RobotActivity>(obj.value(JsonNameName).toString());

					QJsonArray patterns = obj.value(JsonPatternsName).toArray();
					for (int j = 0; j < patterns.size(); j++)
					{
						if (!patterns[j].isObject())
							continue;

						QJsonObject pobj = patterns[j].toObject();

						if (!pobj.contains(JsonMinName) || !pobj.value(JsonMinName).isDouble())
							continue;

						if (!pobj.contains(JsonMaxName) || !pobj.value(JsonMaxName).isDouble())
							continue;

						if (!pobj.contains(JsonNameName) || !pobj.value(JsonNameName).isString())
							continue;

						if (!pobj.contains(JsonPerAllianceName) || !pobj.value(JsonPerAllianceName).isBool())
							continue;

						if (!pobj.contains(JsonTypeName) || !pobj.value(JsonTypeName).isString())
							continue;

						name = pobj.value(JsonNameName).toString();
						minv = pobj.value(JsonMinName).toInt();
						maxv = pobj.value(JsonMaxName).toInt();
						perall = pobj.value(JsonPerAllianceName).toBool();

						if (pobj.value(JsonTypeName).toString() == "enterexit")
						{
							auto p = std::make_shared<SequenceEnterExitPattern>(name, minv, maxv, perall);
							act->addPattern(p);
						}
						else if (pobj.value(JsonTypeName).toString() == "enteridle")
						{
							if (!pobj.contains(JsonIdleValueName) || !pobj.value(JsonIdleValueName).isDouble())
								continue;

							auto p = std::make_shared<SequenceEnterIdlePattern>(name, pobj.value(JsonIdleValueName).toDouble(), minv, maxv, perall);
							act->addPattern(p);
						}
					}

					dm_->addActivity(act);
				}
			}

			QJsonArray JSonDataModelConverter::encodeFieldRegions()
			{
				QJsonArray result;

				for (auto h : dm_->fieldRegions())
				{
					QJsonObject obj;

					obj[JsonAllianceName] = toString(h->alliance());
					obj[JsonNameName] = h->name();
					obj[JsonColorName] = h->color().name(QColor::HexArgb);

					auto rect = std::dynamic_pointer_cast<const RectFieldRegion>(h);
					if (rect != nullptr)
					{
						QJsonObject robj;
						const QRectF& bounds = rect->drawBounds();
						robj[JsonXName] = bounds.x();
						robj[JsonYName] = bounds.y();
						robj[JsonWidthName] = bounds.width();
						robj[JsonHeightName] = bounds.height();
						obj[JsonBoundsName] = robj;
						result.push_back(obj);
						continue;
					}

					auto circle = std::dynamic_pointer_cast<const CircleFieldRegion>(h);
					if (circle != nullptr)
					{
						QJsonObject robj;
						robj[JsonXName] = circle->center().x();
						robj[JsonYName] = circle->center().y();
						robj[JsonRadiusName] = circle->radius();
						obj[JsonCircleName] = robj;
						result.push_back(obj);
						continue;
					}

					auto polygon = std::dynamic_pointer_cast<const PolygonFieldRegion>(h);
					if (polygon != nullptr)
					{
						QJsonArray ptarray;

						for (const QPointF pt : polygon->points())
						{
							QJsonObject ptobj;
							ptobj[JsonXName] = pt.x();
							ptobj[JsonYName] = pt.y();
							ptarray.push_back(ptobj);
						}

						obj[JsonPointsName] = ptarray;
						result.push_back(obj);
						continue;
					}

					assert(false);
				}

				return result;
			}

			void JSonDataModelConverter::decodeFieldRegions(const QJsonArray& array)
			{
				Alliance a;
				QString name;
				QColor color;

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();

					if (!obj.contains(JsonAllianceName) || !obj.value(JsonAllianceName).isString())
						continue;

					if (!obj.contains(JsonNameName) || !obj.value(JsonNameName).isString())
						continue;

					if (!obj.contains(JsonColorName) || !obj.value(JsonColorName).isString())
						continue;

					a = allianceFromString(obj.value(JsonAllianceName).toString());
					name = obj.value(JsonNameName).toString();
					color = QColor(obj.value(JsonColorName).toString());

					if (obj.contains(JsonBoundsName) && obj.value(JsonBoundsName).isObject())
					{
						QJsonObject robj = obj.value(JsonBoundsName).toObject();

						if (!robj.contains(JsonXName) || !robj.value(JsonXName).isDouble())
							continue;

						if (!robj.contains(JsonYName) || !robj.value(JsonYName).isDouble())
							continue;

						if (!robj.contains(JsonWidthName) || !robj.value(JsonWidthName).isDouble())
							continue;

						if (!robj.contains(JsonHeightName) || !robj.value(JsonHeightName).isDouble())
							continue;

						QRectF r(robj.value(JsonXName).toDouble(), robj.value(JsonYName).toDouble(),
							robj.value(JsonWidthName).toDouble(), robj.value(JsonHeightName).toDouble());
						auto h = std::make_shared<RectFieldRegion>(name, color, r, a);
						dm_->addFieldRegion(h);
					}
					else if (obj.contains(JsonCircleName))
					{
						double radius;
						QJsonObject robj = obj.value(JsonCircleName).toObject();

						if (!robj.contains(JsonXName) || !robj.value(JsonXName).isDouble())
							continue;

						if (!robj.contains(JsonYName) || !robj.value(JsonYName).isDouble())
							continue;

						if (!robj.contains(JsonRadiusName) || !robj.value(JsonRadiusName).isDouble())
							continue;

						QPointF pt(robj.value(JsonXName).toDouble(), robj.value(JsonYName).toDouble());
						radius = robj.value(JsonRadiusName).toDouble();
						auto h = std::make_shared<CircleFieldRegion>(name, color, pt, radius, a);
						dm_->addFieldRegion(h);
					}
					else if (obj.contains(JsonPointsName) || obj.value(JsonPointsName).isArray())
					{
						QJsonArray ptarray = obj.value(JsonPointsName).toArray();
						std::vector<QPointF> points;

						for (int i = 0; i < ptarray.size(); i++)
						{
							if (!ptarray[i].isObject())
								continue;

							QJsonObject ptobj = ptarray[i].toObject();
							if (!ptobj.contains(JsonXName) || !ptobj.value(JsonXName).isDouble())
								continue;

							if (!ptobj.contains(JsonYName) || !ptobj.value(JsonYName).isDouble())
								continue;

							QPointF pt(ptobj.value(JsonXName).toDouble(), ptobj.value(JsonYName).toDouble());
							points.push_back(pt);
						}

						auto h = std::make_shared<PolygonFieldRegion>(name, color, points, a);
						dm_->addFieldRegion(h);
					}
				}
			}

			QJsonArray JSonDataModelConverter::encodeColumnOrders()
			{
				QJsonArray array;

				for (auto name : dm_->datasetColumnOrderNames())
				{
					QJsonObject obj;

					obj[JsonNameName] = name;
					QByteArray state, geom;
					dm_->datasetColumnOrder(name, state, geom);
					obj[JsonDatasetColumnsStateName] = encodeBinary(state);
					obj[JsonDatasetColumnsGeomName] = encodeBinary(geom);

					array.push_back(obj);
				}

				return array;
			}

			void JSonDataModelConverter::decodeColumnOrders(const QJsonArray &array)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();

					if (!obj.contains(JsonNameName) || !obj.value(JsonNameName).isString())
						continue;

					if (obj.contains(JsonDatasetColumnsStateName) && obj.value(JsonDatasetColumnsStateName).isString() &&
						obj.contains(JsonDatasetColumnsGeomName) && obj.value(JsonDatasetColumnsGeomName).isString())
					{
						QByteArray state = decodeBinary(obj.value(JsonDatasetColumnsStateName).toString());
						QByteArray geom = decodeBinary(obj.value(JsonDatasetColumnsGeomName).toString());
						dm_->setDatasetColumnOrder(obj.value(JsonNameName).toString(), state, geom);
					}
				}
			}

			QString JSonDataModelConverter::encodeBinary(const QByteArray& data)
			{
				QByteArray a = data.toBase64();
				return QTextCodec::codecForMib(106)->toUnicode(a);
			}

			QByteArray JSonDataModelConverter::decodeBinary(const QString& str)
			{
				QByteArray a = str.toUtf8();
				return QByteArray::fromBase64(a);
			}

			QJsonArray JSonDataModelConverter::encodeStringList(const QStringList& list)
			{
				QJsonArray array;
				
				for (const QString& e : list)
				{
					array.push_back(e);
				}

				return array;
			}

			QStringList JSonDataModelConverter::decodeStringList(const QJsonArray& array)
			{
				QStringList list;

				for (int i = 0; i < array.size(); i++)
				{
					if (array[i].isString())
						list.push_back(array[i].toString());
				}

				return list;
			}

			QJsonArray JSonDataModelConverter::encode(const ConstScoutingDataMapPtr data)
			{
				QJsonArray array;

				if (data != nullptr)
				{
					for (auto entry : *data)
					{
						QJsonObject obj;

						obj[JsonNameName] = entry.first;
						const QVariant& v = entry.second;

						if (v.type() == QVariant::Type::String)
						{
							obj[JsonTypeName] = JsonStringName;
							obj[JsonValueName] = v.toString();
						}
						else if (v.type() == QVariant::Type::Int)
						{
							obj[JsonTypeName] = JsonIntName;
							obj[JsonValueName] = v.toInt();
						}
						else if (v.type() == QVariant::Type::Double)
						{
							obj[JsonTypeName] = JsonDoubleName;
							obj[JsonValueName] = v.toDouble();
						}
						else if (v.type() == QVariant::Type::Bool)
						{
							obj[JsonTypeName] = JsonBoolName;
							obj[JsonValueName] = v.toBool();
						}
						else if (v.type() == QVariant::Type::Double)
						{
							obj[JsonTypeName] = JsonDoubleName;
							obj[JsonValueName] = v.toDouble();
						}
						else {
							qDebug() << "name: " << entry.first << ": variant not handled: " << v;
							assert(false);
						}

						array.push_back(obj);
					}
				}

				return array;
			}

			ScoutingDataMapPtr JSonDataModelConverter::decode(const QJsonArray& data)
			{
				auto result = std::make_shared<std::map<QString, QVariant>>();

				for (int i = 0; i < data.size(); i++) {
					if (!data[i].isObject())
					{
						continue;
					}

					QJsonObject vobj = data[i].toObject();
					if (!vobj.contains(JsonNameName) || !vobj.value(JsonNameName).isString())
					{
						continue;
					}

					if (!vobj.contains(JsonTypeName) || !vobj.value(JsonTypeName).isString())
					{
						continue;
					}

					QString type = vobj.value(JsonTypeName).toString();
					if (type == JsonStringName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isString())
						{
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toString())));
					}
					else if (type == JsonIntName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isDouble())
						{
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toInt())));
					}
					else if (type == JsonDoubleName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isDouble())
						{
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toDouble())));
					}
					else if (type == JsonBoolName)
					{
						if (!vobj.contains("value") || !vobj.value(JsonValueName).isBool())
						{
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toBool())));
					}
					else
					{
						assert(false);
					}
				}

				return result;
			}

			QJsonArray JSonDataModelConverter::encodeFields(std::list<std::shared_ptr<FieldDesc>> fields)
			{
				QJsonArray array;

				for (auto field : fields)
				{
					if (field->isTemporary())
						continue;

					QJsonObject obj;
					QJsonArray strarr;

					obj[JsonNameName] = field->name();
					obj[JsonTypeName] = static_cast<int>(field->type());
					obj[JsonEditableName] = field->isEditable();

					for (const QString& choice : field->choices())
						strarr.push_back(choice);
					obj[JsonChoicesName] = strarr;

					array.push_back(obj);
 				}

				return array;
			}

			std::list<std::shared_ptr<FieldDesc>> JSonDataModelConverter::decodeFields(const QJsonArray &array)
			{
				std::list<std::shared_ptr<FieldDesc>> ret;

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject obj = array[i].toObject();

					if (!obj.contains(JsonNameName) || !obj.contains(JsonTypeName) || !obj.contains(JsonChoicesName) || !obj.contains(JsonEditableName))
						continue;

					if (!obj.value(JsonNameName).isString() || !obj.value(JsonTypeName).isDouble() || !obj.value(JsonChoicesName).isArray() || !obj.value(JsonEditableName).isBool())
						continue;


					bool edit = obj.value(JsonEditableName).toBool();

					QString name = obj.value(JsonNameName).toString();
					FieldDesc::Type t = static_cast<FieldDesc::Type>(obj.value(JsonTypeName).toInt());

					QJsonArray choices = obj.value(JsonChoicesName).toArray();
					auto field = std::make_shared<FieldDesc>(name, t, edit);
					for (auto v : choices)
					{
						if (v.isString())
							field->addChoice(v.toString());
					}

					ret.push_back(field);
				}

				return ret;
			}

			QJsonDocument JSonDataModelConverter::zebraData(const QStringList &keys)
			{
				QJsonDocument doc;
				QJsonObject obj;
				QJsonArray array;

				for(const QString &k : keys)
				{
					auto m = dm_->findMatchByKey(k);
					if (m != nullptr && m->hasZebra())
					{
						QJsonObject zobj;
						zobj[JsonKeyName] = k;
						zobj[JsonDataName] = m->zebra();

						array.push_back(zobj);
					}
				}

				obj[JsonZebraDataName] = array;
				doc.setObject(obj);
				return doc;
			}

			bool JSonDataModelConverter::zebraFromJson(const QJsonDocument &doc)
			{
				if (!doc.isObject())
					return false;

				QJsonObject obj = doc.object();
				if (!obj.contains(JsonZebraDataName) || !obj.value(JsonZebraDataName).isArray())
					return false;

				QJsonArray array = obj.value(JsonZebraDataName).toArray();
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject zobj = array[i].toObject();
					if (!zobj.contains(JsonKeyName) || !zobj.value(JsonKeyName).isString() || !zobj.contains(JsonDataName) || !zobj.value(JsonDataName).isObject())
						continue;

					dm_->assignZebraData(zobj.value(JsonKeyName).toString(), zobj.value(JsonDataName).toObject());
				}

				return true;
			}

			QJsonDocument JSonDataModelConverter::matchDetailData(const QStringList& keys)
			{
				QJsonDocument doc;
				QJsonObject obj;
				QJsonArray array;

				for (const QString& k : keys)
				{
					auto m = dm_->findMatchByKey(k);
					if (m != nullptr && m->hasBlueAllianceData())
					{
						QJsonObject dobj;
						dobj[JsonKeyName] = k;
						QJsonArray darr;

						Alliance c = Alliance::Red;
						for (int slot = 1; slot <= 3; slot++)
						{
							QJsonObject dobj;
							dobj[JsonAllianceName] = toString(c);
							dobj[JsonSlotName] = slot;
							dobj[JsonDataName] = encode(m->extraData(c, slot));
							darr.push_back(dobj);
						}

						c = Alliance::Blue;
						for (int slot = 1; slot <= 3; slot++)
						{
							QJsonObject dobj;
							dobj[JsonAllianceName] = toString(c);
							dobj[JsonSlotName] = slot;
							dobj[JsonDataName] = encode(m->extraData(c, slot));
							darr.push_back(dobj);
						}

						dobj[JsonDataName] = darr;
						array.push_back(dobj);
					}
				}

				obj[JsonMatchDetailName] = array;
				doc.setObject(obj);
				return doc;
			}

			bool JSonDataModelConverter::matchDetailFromJson(const QJsonDocument& doc)
			{
				if (!doc.isObject())
					return false;

				QJsonObject obj = doc.object();
				if (!obj.contains(JsonMatchDetailName) || !obj.value(JsonMatchDetailName).isArray())
					return false;

				QJsonArray array = obj.value(JsonMatchDetailName).toArray();
				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
						continue;

					QJsonObject eobj = array[i].toObject();
					if (!eobj.contains(JsonKeyName) || !eobj.value(JsonKeyName).isString())
						continue;
					QString key = eobj.value(JsonKeyName).toString();

					if (!eobj.contains(JsonDataName) || !eobj.value(JsonDataName).isArray())
						continue;

					QJsonArray darr = eobj.value(JsonDataName).toArray();

					for (int i = 0; i < darr.size(); i++)
					{
						if (!darr[i].isObject())
							continue;

						QJsonObject extra = darr[i].toObject();
						if (!extra.contains(JsonAllianceName) || !extra.value(JsonAllianceName).isString())
							continue;

						if (!extra.contains(JsonSlotName) || !extra.value(JsonSlotName).isDouble())
							continue;

						if (!extra.contains(JsonDataName) || !extra.value(JsonDataName).isArray())
							continue;

						Alliance c = allianceFromString(extra.value(JsonAllianceName).toString());
						int slot = extra.value(JsonSlotName).toInt();
						QJsonArray ex = extra.value(JsonDataName).toArray();

						auto data = decode(ex);
						dm_->addMatchExtraData(key, c, slot, data);
						dm_->addMatchExtraDataFields(data);
					}
				}

				return true;
			}
		}
	}
}
