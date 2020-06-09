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
