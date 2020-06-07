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

#include "PickListView.h"
#include "ApplicationController.h"
#include "PickListGenerator.h"
#include <QElapsedTimer>

class PickListController : public ApplicationController
{
public:
	PickListController(std::shared_ptr<xero::ba::BlueAlliance> ba, int team, int year,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		xero::scouting::views::PickListView* view);
	~PickListController();

	bool isDone() override;
	void run() override;

	bool providesProgress() {
		return true;
	}

	int percentDone() {
		return pcnt_;
	}

	QString htmlPicklist() const {
		return gen_->picklist();
	}

	QString htmlRobotCapabilities() const {
		return gen_->robotCapabilities();
	}

private:
	void computeOneTeam(const QString& key);
	void logGenMessage(const QString& msg);

private:
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	int pcnt_;
	PickListGenerator* gen_;
	xero::scouting::views::PickListView* view_;
	QElapsedTimer timer_;
	bool started_;
};

