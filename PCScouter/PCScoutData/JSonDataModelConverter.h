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
				QJsonDocument zebraData(const QStringList& keys);
				QJsonDocument matchDetailData(const QStringList& keys);

				bool coreFromJson(const QJsonObject& obj);
				bool scoutingFromJson(const QJsonObject& obj, TabletIdentity& id, QStringList& pits, QStringList& matches);
				bool scoutingFromJsonFull(const QJsonObject& obj);

				bool coreFromJson(const QJsonDocument& obj);
				bool scoutingFromJson(const QJsonDocument& obj, TabletIdentity&, QStringList& pits, QStringList& matches);
				bool allFromJson(const QJsonDocument& obj);
				bool scoutingFromAllJson(const QJsonDocument& obj);
				bool zebraFromJson(const QJsonDocument& doc);
				bool matchDetailFromJson(const QJsonDocument& doc);

				const QStringList& errors() {
					return errors_;
				}

				static ScoutingDataMapPtr decode(const QJsonArray& data);
				static QJsonArray encode(const ConstScoutingDataMapPtr data);

				static QJsonArray encodeFields(std::list<std::shared_ptr<FieldDesc>> list);
				std::list<std::shared_ptr<FieldDesc>> decodeFields(const QJsonArray& array);

				static bool peekUUID(const QJsonDocument &doc, QUuid& uuid);

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

				QString encodeBinary(const QByteArray& data);
				QByteArray decodeBinary(const QString& str);

				QJsonArray encodeColumnOrders();
				void decodeColumnOrders(const QJsonArray& array);

				QJsonArray encodeFieldRegions();
				void decodeFieldRegions(const QJsonArray& array);

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
