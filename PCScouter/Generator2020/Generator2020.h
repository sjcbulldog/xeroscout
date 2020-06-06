#pragma once

#include "DataModelMatch.h"
#include "DataModelTeam.h"
#include "ScoutingDataMap.h"
#include "ScoutingForm.h"

#include "generator2020_global.h"

GENERATOR2020_EXPORT std::vector<xero::scouting::datamodel::ScoutingDataMapPtr>
generateMatch(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match,
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

GENERATOR2020_EXPORT xero::scouting::datamodel::ScoutingDataMapPtr
generateTeam(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> team,
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);
