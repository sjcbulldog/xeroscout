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

#include "ApplicationController.h"
#include "BlueAllianceEngine.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ApplicationController::ApplicationController(std::shared_ptr<BlueAlliance> ba, std::shared_ptr<ScoutingDataModel> dm)
{
	ba_ = ba;
	dm_ = dm;
	display_initialized_ = false;

	if (dm_ != nullptr)
	{
		// restoreBlueAllianceData();
	}
}

ApplicationController::~ApplicationController()
{
}

void ApplicationController::restoreBlueAllianceData()
{
	for (auto team : dm_->teams())
	{
		ba_->getEngine().createTeam(team->key(), team->number(), team->nick(), team->name(), 
				team->city(), team->state(), team->country());
	}

	for (auto match : dm_->matches())
	{
		std::shared_ptr<MatchAlliance> red = std::make_shared<MatchAlliance>();
		std::shared_ptr<MatchAlliance> blue = std::make_shared<MatchAlliance>();
		int etime = match->eventTime();

		Alliance c;

		c = Alliance::Red;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = match->team(c, slot);
			red->addTeam(team);
		}

		c = Alliance::Blue;
		for (int slot = 1; slot <= 3; slot++)
		{
			QString team = match->team(c, slot);
			blue->addTeam(team);
		}
		auto m = ba_->getEngine().createMatch(dm_->evkey(), match->key(), match->compType(), match->set(), match->match(), etime, 0, 0, red, blue);

		if (match->hasZebra())
			m->setZebraData(match->zebra());
	}
}