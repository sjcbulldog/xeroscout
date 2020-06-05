#pragma once

#include "DataModelMatch.h"
#include "DataModelTeam.h"
#include "ScoutingDataMap.h"

#include "generator2020_global.h"

class GENERATOR2020_EXPORT Generator2020
{
public:
    Generator2020();

    static bool generate(std::shared_ptr<xero::scouting::datamodel::DataModelMatch> match,
        std::shared_ptr < xero::scouting::datamodel::ScoutingDataMapPtr> ptr);

    static bool generate(std::shared_ptr<xero::scouting::datamodel::DataModelTeam> match,
        std::shared_ptr < xero::scouting::datamodel::ScoutingDataMapPtr> ptr);

private:
};
