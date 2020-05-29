#include "ImportZebraDataController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportZebraDataController::ImportZebraDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) : ApplicationController(ba)
{
	dm_ = dm;
	state_ = State::Start;
}

ImportZebraDataController::~ImportZebraDataController()
{
}

bool ImportZebraDataController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void ImportZebraDataController::run()
{
	if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		emit errorMessage("cannot create a new event, cannot reach the blue alliance");
		emit logMessage("cannot create a new event, cannot reach the blue alliance");

		emit complete(true);

		state_ = State::Error;
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Initializing)
	{
		//
		// Wait for the blue alliance engine to either be up or down.  There is a timeout
		// in the blue alliance
		//
	}
	else if (blueAlliance()->isIdle())
	{
		//
		// The primary purpose of this state machine is to wait on the blue alliance to finish
		// an operation and then start the next one.  Therefore we only transition when the blue alliance
		// engine is idle, meaning it has completed that last thing we asked it to do
		//

		switch (state_)
		{
		case State::Start:
			blueAlliance()->requestMatches(dm_->evkey());
			state_ = State::WaitingForMatches;
			break;

		case State::WaitingForMatches:
		{
			QStringList keys;

			for (auto m : dm_->matches())
				keys.push_back(m->key());

			blueAlliance()->requestZebraData(keys);
			state_ = State::WaitingForZebra;
		}
		break;

		case State::WaitingForZebra:
			gotZebra();
			break;
		}
	}
}

void ImportZebraDataController::gotZebra()
{
	dm_->blockSignals(true);

	//
	// Move the zebra data from the blue alliance engine to the data model
	//
	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches) {
		if (!pair.second->zebraData().isEmpty())
		{
			auto m = dm_->findMatchByKey(pair.first);
			if (m != nullptr)
			{
				dm_->assignZebraData(m->key(), pair.second->zebraData());
			}
		}
	}

	dm_->blockSignals(false);

	emit complete(false);
	state_ = State::Done;

	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::ZebraDataAdded);
}