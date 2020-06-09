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