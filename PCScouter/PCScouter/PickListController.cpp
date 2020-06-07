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

#include "PickListController.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

PickListController::PickListController(std::shared_ptr<BlueAlliance> ba, int team, int year,
	std::shared_ptr<ScoutingDataModel> dm, xero::scouting::views::PickListView* view) : ApplicationController(ba)
{
	view_ = view;
	dm_ = dm;

	gen_ = new PickListGenerator(team, dm, year);
	connect(gen_, &PickListGenerator::logMessage, this, &PickListController::logMessage);
	started_ = false;
}

PickListController::~PickListController()
{
	delete gen_;
}

bool PickListController::isDone()
{
	if (gen_->isDone())
	{
		QString elapsed = "Elapsed time " + QString::number(timer_.elapsed() / 1000) + " seconds";
		emit logMessage(elapsed);
		emit complete(false);
		return true;
	}

	return false;
}

void PickListController::run()
{
	if (!started_)
	{
		timer_.start();
		gen_->start();
		started_ = true;
	}
	else
	{
		qint64 total = 60 * 1000;
		pcnt_ = (timer_.elapsed() * 100) / total;

		if (pcnt_ > 99)
			pcnt_ = 99;
	}
}

void PickListController::logGenMessage(const QString& msg)
{
	emit logMessage(msg);
}
