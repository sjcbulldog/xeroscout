#pragma once

#include "BlueAlliance.h"
#include "BlueAllianceEngine.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include "ScoutingForm.h"
#include <memory>

class DataModelBuilder
{
public:
	DataModelBuilder() = delete;
	~DataModelBuilder() = delete;

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> 
		buildModel(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<xero::scouting::datamodel::ScoutingForm> pit, 
		std::shared_ptr<xero::scouting::datamodel::ScoutingForm> match, const QString& evkey, QString& error);

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
		buildModel(xero::ba::BlueAllianceEngine& engine, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> pit,
			std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match, const QString& evkey, QString& error);

	static std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel>
		buildModel(xero::ba::BlueAllianceEngine& engine, const QString &pitform, const QString &matchform, 
		const QString& evkey, QString& error);

	static void jsonToPropMap(const QJsonObject& obj, const QString& alliance, xero::scouting::datamodel::ScoutingDataMapPtr map);
};

