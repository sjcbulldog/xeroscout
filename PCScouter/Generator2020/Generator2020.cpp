#include "Generator2020.h"

Generator2020::Generator2020()
{
}

bool Generator2020::generate(std::shared_ptr<xero::scouting::datamodel::DataModelMatch> match,
    std::shared_ptr < xero::scouting::datamodel::ScoutingDataMapPtr> ptr)
{
    return true;
}

bool Generator2020::generate(std::shared_ptr<xero::scouting::datamodel::DataModelTeam> match,
    std::shared_ptr < xero::scouting::datamodel::ScoutingDataMapPtr> ptr)
{
    return true;
}
