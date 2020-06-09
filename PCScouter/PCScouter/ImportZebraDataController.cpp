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

#include "ImportZebraDataController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportZebraDataController::ImportZebraDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) : ApplicationController(ba, dm)
{
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
			if (blueAlliance()->getEngine().matchCount() == 0)
			{
				blueAlliance()->requestMatches(dataModel()->evkey());
				state_ = State::WaitingForMatches;
			}
			else
			{
				QStringList keys;

				for (auto m : dataModel()->matches())
				{
					if (!m->hasZebra())
						keys.push_back(m->key());
				}

				blueAlliance()->requestZebraData(keys);
				state_ = State::WaitingForZebra;
			}
			break;

		case State::WaitingForMatches:
		{
			QStringList keys;

			for (auto m : dataModel()->matches())
			{
				if (!m->hasZebra())
					keys.push_back(m->key());
			}

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
	dataModel()->blockSignals(true);

	//
	// Move the zebra data from the blue alliance engine to the data model
	//
	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches) {
		if (!pair.second->zebraData().isEmpty())
		{
			auto m = dataModel()->findMatchByKey(pair.first);
			if (m != nullptr)
			{
				dataModel()->assignZebraData(m->key(), pair.second->zebraData());
			}
		}
	}

	dataModel()->blockSignals(false);

	emit complete(false);
	state_ = State::Done;

	dataModel()->emitChangedSignal(ScoutingDataModel::ChangeType::ZebraDataAdded);
}