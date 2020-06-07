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
		emit errorMessage("cannot get zebra data, cannot reach the blue alliance");
		emit logMessage("cannot get zebra data, cannot reach the blue alliance");

		state_ = State::Error;

		emit complete(true);
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Initializing)
	{
		//
		// Wait for the blue alliance engine to either be up or down.  There is a timeout
		// in the blue alliance
		//
	}
	else if (blueAlliance()->state() == BlueAlliance::EngineState::Down)
	{
		//
		// The network went down in the middle
		//
		emit errorMessage("The BlueAlliance failed during the operation");
		emit logMessage("The BlueAlliance failed during the operation");
		state_ = State::Error;
		emit complete(true);
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