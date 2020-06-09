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

#include "ApplicationController.h"
#include "ScoutingDataModel.h"

class AllTeamSummaryController : public ApplicationController
{
public:
	AllTeamSummaryController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::datamodel::ScoutingDataSet& ds);
	~AllTeamSummaryController();

	bool isDone() override;
	void run() override;

	bool providesProgress() {
		return true;
	}

	int percentDone() {
		return pcnt_;
	}

private:
	void computeOneTeam(const QString& key);

private:
	xero::scouting::datamodel::ScoutingDataSet& ds_;
	int pcnt_;
	QStringList keys_;
	int index_;
	bool headers_;
};

