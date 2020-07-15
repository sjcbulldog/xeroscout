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

#include "PickListController.h"
#include <QSettings>

using namespace xero::ba;
using namespace xero::scouting::datamodel;

PickListController::PickListController(std::shared_ptr<BlueAlliance> ba, int team, int year,
	std::shared_ptr<ScoutingDataModel> dm, const QString &name) : ApplicationController(ba, dm)
{
	gen_ = new PickListGenerator(team, dm, name);
	connect(gen_, &PickListGenerator::logMessage, this, &PickListController::logMessage);
	started_ = false;

	total_time_ = 60.0;
	QSettings settings;
	if (settings.contains("picklisttime") && settings.contains("picklistcount"))
	{
		total_time_ = settings.value("picklisttime").toDouble() / settings.value("picklistcount").toInt();
	}
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

		QSettings settings;
		double total;
		int count;
		if (settings.contains("picklisttime") && settings.contains("picklistcount"))
		{
			total = settings.value("picklisttime").toDouble();
			count = settings.value("picklistcount").toInt();
		}

		total += timer_.elapsed() / 1000.0;
		count++;

		settings.setValue("picklisttime", total);
		settings.setValue("picklistcount", count);
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
		pcnt_ = (timer_.elapsed() * 100) / (total_time_ * 1000);

		if (pcnt_ > 99)
			pcnt_ = 99;
	}
}

void PickListController::logGenMessage(const QString& msg)
{
	emit logMessage(msg);
}
