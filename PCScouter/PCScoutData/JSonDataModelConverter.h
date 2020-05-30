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

#pragma once

#include "ScoutingDataMap.h"
#include "FieldDesc.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ScoutingDataModel;
			class TabletIdentity;

			class JSonDataModelConverter
			{
			public:
				JSonDataModelConverter(ScoutingDataModel* dm);
				virtual ~JSonDataModelConverter();

				QJsonObject coreToJson(bool extra);
				QJsonObject scoutingToJson(const TabletIdentity* sender, const QString& tablet);
				QJsonObject scoutingToJsonFull();

				QJsonDocument coreToJsonDoc(bool extra);
				QJsonDocument scoutingToJsonDoc(const TabletIdentity* sender, const QString& tablet);
				QJsonDocument allToJsonDoc();
				QJsonDocument tabletToJsonDoc(const QString &tablet);

				bool coreFromJson(const QJsonObject& obj);
				bool scoutingFromJson(const QJsonObject& obj, TabletIdentity& id, QStringList& pits, QStringList& matches);
				bool scoutingFromJsonFull(const QJsonObject& obj);

				bool coreFromJson(const QJsonDocument& obj);
				bool scoutingFromJson(const QJsonDocument& obj, TabletIdentity&, QStringList& pits, QStringList& matches);
				bool allFromJson(const QJsonDocument& obj);
				bool scoutingFromAllJson(const QJsonDocument& obj);

				const QStringList& errors() {
					return errors_;
				}

				static ScoutingDataMapPtr decode(const QJsonArray& data);
				static QJsonArray encode(const ConstScoutingDataMapPtr data);

				static QJsonArray encodeFields(std::list<std::shared_ptr<FieldDesc>> list);
				std::list<std::shared_ptr<FieldDesc>> decodeFields(const QJsonArray& array);

			private:
				bool coreFromJsonV1(const QJsonObject& obj);
				bool extractNameAndKey(const QJsonObject& obj, int version);
				bool extractRegisteredTabletList(const QJsonObject& obj, int version);
				bool extractScoutingForms(const QJsonObject& obj, int version);
				bool extractTeams(const QJsonObject& obj, int version);
				bool extractMatches(const QJsonObject& obj, int version);
				bool extractTabletLists(const QJsonObject& obj, int version);

				bool scoutingFromJsonV1(const QJsonObject& obj, TabletIdentity& id, QStringList& pits, QStringList& matches);
				bool extractTabletID(const QJsonObject& obj, int version, TabletIdentity& id);
				bool extractPits(const QJsonObject& obj, int version, const QString& tablet, QStringList& pits);
				bool extractMatches(const QJsonObject& obj, int version, const QString& tablet, QStringList& matches);
				bool extractTeamSummaries(const QJsonArray& array);

				bool scoutingFromJsonFullV1(const QJsonObject& obj);
				bool scoutingFromJsonFullV1Matches(const QJsonArray& obj);
				bool scoutingFromJsonFullV1Teams(const QJsonArray& obj);

				bool extractHistory(const QJsonObject& obj);
				bool extractHistoryV1(const QJsonObject& obj);
				QJsonObject historyToJson();

				QJsonArray encodeScoutingData(const std::list<ConstScoutingDataMapPtr>& data);

				QJsonArray encodeStringList(const QStringList& list);
				QStringList decodeStringList(const QJsonArray& array);


			private:
				ScoutingDataModel* dm_;
				QStringList errors_;

				int scouting_full_version_;
				int scouting_version_;
				int history_version_;
				int core_version_ ;
			};
		}
	}
}
