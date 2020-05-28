#include "TeamSummaryController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

TeamSummaryController::TeamSummaryController(std::shared_ptr<BlueAlliance> ba,
	std::shared_ptr<ScoutingDataModel> dm, ScoutingDataSet &ds) : ApplicationController(ba)
{
	done_ = false;
	dm_ = dm;
	//gen_ = new AllTeamSummaryGenerator(dm, ds);

	// connect(gen_, &AllTeamSummaryGenerator::reportProgress, this, &TeamSummaryController::progress);
	// gen_->start();
}

TeamSummaryController::~TeamSummaryController()
{
}

void TeamSummaryController::progress(int pcnt)
{
	pcnt_ = pcnt;
}

bool TeamSummaryController::isDone()
{
	return done_;
}

void TeamSummaryController::run()
{
#ifdef TEAM_SUMMARY_NOT_READY
	gen_->run();

	if (gen_->done())
	{
		if (gen_->hasError())
		{
			emit logMessage(gen_->error());
			emit errorMessage(gen_->error());
		}
		else
		{
		}

		emit complete(false);
		done_ = true;
	}
#endif

	emit complete(false);
	done_ = true;
}
