//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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
