#include "JSonDataModelConverter.h"
#include "ScoutDataJsonNames.h"
#include "ScoutingDataModel.h"
#include <QJsonArray>

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
				payload[JsonEvNameName] = dm_->evname();
				payload[JsonMatchScoutingFormName] = dm_->matchScoutingForm()->obj();
				payload[JsonPitScoutingFormName] = dm_->pitScoutingForm()->obj();

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
					obj[JsonNameName] = team->name();
					obj[JsonAssignName] = team->tablet();

					if (team->hasRankingInfo())
						obj[JsonRankingBlob] = team->rankingInfo();

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
						QString t = m->team(DataModelMatch::Alliance::Red, i);
						QString assign = m->tablet(DataModelMatch::Alliance::Red, i);

						red.push_back(t);
						red.push_back(assign);
					}
					obj[JsonRedName] = red;

					QJsonArray blue;
					for (int i = 1; i <= 3 ; i++)
					{
						QString t = m->team(DataModelMatch::Alliance::Blue, i);
						QString assign = m->tablet(DataModelMatch::Alliance::Blue, i);

						blue.push_back(t);
						blue.push_back(assign);
					}
					obj[JsonBlueName] = blue;

					QJsonArray badatared;
					QJsonArray badatablue;

					if (extra)
					{
						for (int i = 1; i <= 3; i++) {
							badatared.push_back(encode(m->blueAllianceData(DataModelMatch::Alliance::Red, i)));
							badatablue.push_back(encode(m->blueAllianceData(DataModelMatch::Alliance::Blue, i)));
						}
					}

					obj[JsonBaDataRedName] = badatared;
					obj[JsonBaDataBlueName] = badatablue;

					if (extra)
						obj[JsonZebraDataName] = m->zebra();

					matches.push_back(obj);
				}
				payload[JsonMatchesName] = matches;

				QJsonArray match_tablets;
				for (auto mt : dm_->matchTablets())
					match_tablets.push_back(mt);
				payload[JsonMatchTabletListName] = match_tablets;

				QJsonArray pit_tablets;
				for (auto pt : dm_->pitTablets())
					pit_tablets.push_back(pt);
				payload[JsonPitTabletListName] = pit_tablets;

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
					for (auto sd : t->pitScoutingDataList())
						arr.push_back(encode(sd));
					tm[JsonPitsName] = arr;

					pits.push_back(tm);
				}
				
				for (auto m : dm_->matches())
				{
					QJsonObject mt;
					QJsonArray arr;

					mt[JsonKeyName] = m->key();

					DataModelMatch::Alliance c = DataModelMatch::Alliance::Red;
					for (int i = 1; i <= 3; i++)
					{
						QString key = DataModelMatch::toString(c) + QString::number(i);
						mt[key] = encodeScoutingData(m->scoutingDataList(c, i));
					}

					c = DataModelMatch::Alliance::Blue;
					for (int i = 1; i <= 3; i++)
					{
						QString key = DataModelMatch::toString(c) + QString::number(i);
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
					if (t->tablet() == tablet && t->pitScoutingData() != nullptr)
					{
						QJsonObject pit;
						pit[JsonTeamName] = t->key();
						pit[JsonResultName] = encode(t->pitScoutingData());

						pits.push_back(pit);
					}
				}
				obj[JsonPitsName] = pits;

				QJsonArray matches;
				for (auto m : dm_->matches()) {
					DataModelMatch::Alliance c;
					int slot;

					if (m->tabletToAllianceSlot(tablet, c, slot))
					{
						if (m->hasScoutingData(c, slot)) {
							QJsonObject match;
							match[JsonKeyName] = m->key();
							match[JsonAllianceName] = DataModelMatch::toString(c);
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

				obj[JsonTypeName] = JsonTableDatatName;
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
				obj[JsonGraphViews] = dm_->graphDescriptors().generateJSON();
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

				return true;
			}

			bool JSonDataModelConverter::extractNameAndKey(const QJsonObject& obj, int version)
			{
				if (!obj.contains(JsonEvKeyName) || !obj.value(JsonEvKeyName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvKeyName) + "'");
					return false;
				}

				dm_->ev_key_ = obj[JsonEvKeyName].toString();

				if (!obj.contains(JsonEvNameName) || !obj.value(JsonEvNameName).isString()) {
					errors_.push_back("expected string field '" + QString(JsonEvNameName) + "'");
					return false;
				}
				dm_->event_name_ = obj[JsonEvNameName].toString();

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

				dm_->pit_scouting_form_ = std::make_shared<ScoutingForm>(obj[JsonPitScoutingFormName].toObject());
				if (!dm_->pit_scouting_form_->isOK()) {
					for (const QString& err : dm_->match_scouting_form_->errors())
						errors_.push_back(err);
					return false;
				}

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

					QString key = tobj[JsonKeyName].toString();
					int number = tobj[JsonNumberName].toInt();
					QString name = tobj[JsonNameName].toString();

					auto t = dm_->addTeam(key, number, name);
					dm_->assignTeamTablet(key, tobj[JsonAssignName].toString());

					if (tobj.contains(JsonRankingBlob) && tobj.value(JsonRankingBlob).isObject())
					{
						dm_->setTeamRanking(key, tobj.value(JsonRankingBlob).toObject());
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

					if (!mobj.contains(JsonBaDataRedName) || !mobj.value(JsonBaDataRedName).isArray()) {
						errors_.push_back("expected JSON array field 'badata'");
						return false;
					}

					if (!mobj.contains(JsonBaDataBlueName) || !mobj.value(JsonBaDataBlueName).isArray()) {
						errors_.push_back("expected JSON array field 'badata'");
						return false;
					}

					auto dm = dm_->addMatch(mobj.value(JsonKeyName).toString(), mobj.value(JsonCompTypeName).toString(), mobj.value(JsonSetName).toInt(), mobj.value(JsonMatchName).toInt(), mobj.value(JsonEtimeName).toInt());
					dm->clearTeams();

					QJsonArray red = mobj.value(JsonRedName).toArray();
					for (int i = 0; i < red.size(); i += 2) {
						int slot = i / 2 + 1;
						if (!red[i].isString() || !red[i + 1].isString())
						{
							errors_.push_back("expected field 'red' to be an array of strings (teamkeys and tablets)");
							return false;
						}

						dm->addTeam(DataModelMatch::Alliance::Red, red[i].toString());
						dm->setTablet(DataModelMatch::Alliance::Red, slot, red[i + 1].toString());
					}

					QJsonArray blue = mobj.value(JsonBlueName).toArray();
					for (int i = 0; i < blue.size(); i += 2) {
						int slot = i / 2 + 1;
						if (!red[i].isString() || !red[i + 1].isString())
						{
							errors_.push_back("expected field 'red' to be an array of strings (teamkeys and tablets)");
							return false;
						}

						dm->addTeam(DataModelMatch::Alliance::Blue, blue[i].toString());
						dm->setTablet(DataModelMatch::Alliance::Blue, slot, blue[i + 1].toString());
					}

					QJsonArray badatared = mobj.value(JsonBaDataRedName).toArray();
					QJsonArray badatablue = mobj.value(JsonBaDataBlueName).toArray();

					if (badatablue.size() == 3 && badatared.size() == 3) {
						for (int i = 1; i <= 3; i++) {
							dm->setBlueAllianceData(DataModelMatch::Alliance::Red, i, decode(badatared[i - 1].toArray()));
							dm->setBlueAllianceData(DataModelMatch::Alliance::Blue, i, decode(badatablue[i - 1].toArray()));
						}
					}

					if (mobj.contains(JsonZebraDataName) && mobj.value(JsonZebraDataName).isObject())
						dm->setZebra(mobj.value(JsonZebraDataName).toObject());
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

				if (!obj.contains(JsonPitTabletListName) || !obj.value(JsonPitTabletListName).isArray()) {
					errors_.push_back("expected array field 'pittablets'");
					return false;
				}

				a = obj[JsonPitTabletListName].toArray();
				for (int i = 0; i < a.size(); i++) {
					if (a[i].isString())
						dm_->pit_tablets_.push_back(a[i].toString());
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

					DataModelMatch::Alliance c = DataModelMatch::Alliance::Red;
					for (int i = 1; i <= 3; i++)
					{
						QString key = DataModelMatch::toString(c) + QString::number(i);
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

					c = DataModelMatch::Alliance::Blue;
					for (int i = 1; i <= 3; i++)
					{
						QString key = DataModelMatch::toString(c) + QString::number(i);
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

			bool JSonDataModelConverter::scoutingFromJsonFullV1Pits(const QJsonArray& arr)
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
						errors_.push_back("expected pit scouting data in scouting array");
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

				if (!scoutingFromJsonFullV1Pits(obj.value(JsonPitsName).toArray()))
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
						errors_.push_back("scouting data for pits was corrupt, expected string field 'team' in pit scouting result");
						return false;
					}

					if (!pitres.contains(JsonResultName) || !pitres.value(JsonResultName).isArray())
					{
						errors_.push_back("scouting data for pits was corrupt, expected array field 'result' in pit scouting result");
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
						errors_.push_back("scouting data for pits was corrupt, expected string field 'key' in pit scouting result");
						return false;
					}

					if (!matchres.contains("alliance") || !matchres.value("alliance").isString())
					{
						errors_.push_back("scouting data for pits was corrupt, expected string field 'alliance' in pit scouting result");
						return false;
					}

					if (!matchres.contains("slot") || !matchres.value("slot").isDouble())
					{
						errors_.push_back("scouting data for pits was corrupt, expected integer field 'slot' in pit scouting result");
						return false;
					}

					if (!matchres.contains(JsonResultName) || !matchres.value(JsonResultName).isArray())
					{
						errors_.push_back("scouting data for pits was corrupt, expected JSON array field 'result' in pit scouting result");
						return false;
					}

					auto m = dm_->findMatchByKey(matchres.value(JsonKeyName).toString());
					DataModelMatch::Alliance c = DataModelMatch::allianceFromString(matchres.value(JsonAllianceName).toString());

					if (dm_->setMatchScoutingData(m->key(), c, matchres.value(JsonSlotName).toInt(), decode(matchres.value(JsonResultName).toArray())))
						added.push_back(matchres.value(JsonKeyName).toString());
				}

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

				if (obj.value(JsonTypeName).toString() != JsonAllName && obj.value(JsonTypeName).toString() != JsonTableDatatName) {
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

				if (obj.value(JsonTypeName).toString() != JsonAllName && obj.value(JsonTypeName).toString() != JsonTableDatatName) {
					errors_.push_back("JSON document is not a 'all' document");
					return false;
				}

				if (obj.value(JsonTypeName).toString() == JsonTableDatatName)
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
				}

				if (!scoutingFromJsonFull(obj.value(JsonScoutingName).toObject()))
					return false;

				if (obj.contains(JsonGraphViews) && obj.value(JsonGraphViews).isArray())
				{
					QString err;
					if (!dm_->setGraphViews(obj.value(JsonGraphViews).toArray(), err))
					{
						errors_.push_back(err);
						return false;
					}
				}

				return true;
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
						else {
							assert(false);
							qDebug() << "variant not handled: " << v;
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
						qDebug() << "bad data in encoded scouting result - not an object";
						continue;
					}

					QJsonObject vobj = data[i].toObject();
					if (!vobj.contains(JsonNameName) || !vobj.value(JsonNameName).isString())
					{
						qDebug() << "bad data in encoded scouting result - missing string field 'name'";
						continue;
					}

					if (!vobj.contains(JsonTypeName) || !vobj.value(JsonTypeName).isString())
					{
						qDebug() << "bad data in encoded scouting result - missing string field 'type'";
						continue;
					}

					QString type = vobj.value(JsonTypeName).toString();
					if (type == JsonStringName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isString())
						{
							qDebug() << "bad data in encoded scouting result - missing string field 'value'";
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toString())));
					}
					else if (type == JsonIntName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isDouble())
						{
							qDebug() << "bad data in encoded scouting result - missing integer field 'value'";
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toInt())));
					}
					else if (type == JsonDoubleName)
					{
						if (!vobj.contains(JsonValueName) || !vobj.value(JsonValueName).isDouble())
						{
							qDebug() << "bad data in encoded scouting result - missing double field 'value'";
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toDouble())));
					}
					else if (type == JsonBoolName)
					{
						if (!vobj.contains("value") || !vobj.value(JsonValueName).isBool())
						{
							qDebug() << "bad data in encoded scouting result - missing boolean field 'value'";
							continue;
						}
						result->insert(std::make_pair(vobj.value(JsonNameName).toString(), QVariant(vobj.value(JsonValueName).toBool())));
					}
					else
					{
						assert(false);
						qDebug() << "decode data type not handled '" << type << "'";
					}
				}

				return result;
			}
		}
	}
}
