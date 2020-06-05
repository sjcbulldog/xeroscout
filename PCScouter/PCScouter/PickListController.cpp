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
