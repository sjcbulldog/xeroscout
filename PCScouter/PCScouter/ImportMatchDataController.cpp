#include "ImportMatchDataController.h"
#include "BlueAllianceEngine.h"
#include "DataModelBuilder.h"
#include "TestDataInjector.h"

using namespace xero::ba;
using namespace xero::scouting::datamodel;

ImportMatchDataController::ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int maxmatch) : ApplicationController(ba)
{
	dm_ = dm;
	state_ = State::Start;
	maxmatch_ = maxmatch;
}

ImportMatchDataController::~ImportMatchDataController()
{
}

bool ImportMatchDataController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void ImportMatchDataController::run()
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
				{
					if (m->match() <= maxmatch_)
						keys.push_back(m->key());
				}

				state_ = State::WaitingForDetail;
				blueAlliance()->requestMatchesDetails(keys);
			}
			break;

		case State::WaitingForTeams:
			state_ = State::WaitingForRankings;
			blueAlliance()->requestRankings(dm_->evkey());
			break;

		case State::WaitingForDetail:
			gotDetail();
			break;

		case State::WaitingForRankings:
			gotRankings();
			break;
		}
	}
}

void ImportMatchDataController::breakoutBlueAlliancePerRobotData(std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>>& data)
{
	for (auto m : dm_->matches()) {
		if (m->match() <= maxmatch_)
		{
			breakOutBAData(m, Alliance::Red, data[m->key()].first);
			breakOutBAData(m, Alliance::Blue, data[m->key()].second);
		}
	}
}

void ImportMatchDataController::breakOutBAData(std::shared_ptr<const DataModelMatch> m, Alliance c, ScoutingDataMapPtr data)
{
	ScoutingDataMapPtr newdata1 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata2 = std::make_shared<ScoutingDataMap>();
	ScoutingDataMapPtr newdata3 = std::make_shared<ScoutingDataMap>();

	for (auto pair : *data)
	{
		if (pair.first.endsWith("Robot1")) {
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata1->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot2"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata2->insert_or_assign(name, pair.second);
		}
		else if (pair.first.endsWith("Robot3"))
		{
			QString name = pair.first.mid(0, pair.first.length() - 6);
			newdata3->insert_or_assign(name, pair.second);
		}
		else
		{
			newdata1->insert_or_assign(pair.first, pair.second);
			newdata2->insert_or_assign(pair.first, pair.second);
			newdata3->insert_or_assign(pair.first, pair.second);
		}
	}

	dm_->addMatchExtraData(m->key(), c, 1, newdata1);
	dm_->addMatchExtraData(m->key(), c, 2, newdata2);
	dm_->addMatchExtraData(m->key(), c, 3, newdata3);

	processFieldDesc(newdata1);
	processFieldDesc(newdata2);
	processFieldDesc(newdata3);
}

void ImportMatchDataController::setExtraFields()
{
	for (auto pair : strings_)
	{
		if (pair.second.count() > 6) {
			// 
			// Treat as a normal string
			//
			std::shared_ptr<FieldDesc> desc = std::make_shared<FieldDesc>(pair.first, FieldDesc::Type::String, false);
			fields_.push_back(desc);
		}
		else
		{
			//
			// Treat as a choice
			//
			std::shared_ptr<FieldDesc> desc = std::make_shared<FieldDesc>(pair.first, pair.second, false);
			fields_.push_back(desc);
		}
	}

	dm_->addMatchExtraFields(fields_);
}

void ImportMatchDataController::processFieldDesc(ScoutingDataMapPtr data)
{
	std::shared_ptr<FieldDesc> desc;

	for (auto d : *data) {
		desc = findField(d.first);

		if (d.second.type() == QVariant::Int)
		{
			if (desc == nullptr)
			{
				fields_.push_back(std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Integer, false));
			}
			else
			{
				assert(desc->type() == FieldDesc::Type::Integer);
			}
		}
		else if (d.second.type() == QVariant::Bool)
		{
			if (desc == nullptr)
			{
				fields_.push_back(std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Boolean, false));
			}
			else
			{
				assert(desc->type() == FieldDesc::Type::Boolean);
			}
		}
		else if (d.second.type() == QVariant::Double)
		{
			if (desc == nullptr)
			{
				fields_.push_back(std::make_shared<FieldDesc>(d.first, FieldDesc::Type::Double, false));
			}
			else
			{
				assert(desc->type() == FieldDesc::Type::Double);
			}
		}
		else if (d.second.type() == QVariant::String)
		{
			QStringList list;
			auto it = strings_.find(d.first);
			if (it != strings_.end())
				list = it->second;

			if (!list.contains(d.second.toString()))
				list.push_back(d.second.toString());

			strings_.insert_or_assign(d.first, list);
		}
	}
}

void ImportMatchDataController::gotDetail()
{
	bool baFieldsAdded = false;

	dm_->blockSignals(true);

	//
	// Move the data from the blue alliance engine to the data model
	//
	std::map<QString, std::pair<ScoutingDataMapPtr, ScoutingDataMapPtr>> badata;

	auto matches = blueAlliance()->getEngine().matches();
	for (auto pair : matches) {
		if (!pair.second->scoreBreakdown().isEmpty())
		{
			auto m = dm_->findMatchByKey(pair.first);
			if (m != nullptr && m->match() <= maxmatch_)
			{
				auto redmap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "red", redmap);

				auto bluemap = std::make_shared<ScoutingDataMap>();
				DataModelBuilder::jsonToPropMap(pair.second->scoreBreakdown(), "blue", bluemap);

				badata.insert_or_assign(m->key(), std::make_pair(redmap, bluemap));
			}
		}
	}

	breakoutBlueAlliancePerRobotData(badata);
	setExtraFields();

	dm_->blockSignals(false);

	QStringList teams;
	auto& bateams = blueAlliance()->getEngine().teams();

	for (auto team : dm_->teams())
	{
		auto it = bateams.find(team->key());
		if (it == bateams.end())
			teams.push_back(team->key());
	}

	if (teams.count() > 0)
	{
		state_ = State::WaitingForTeams;
		blueAlliance()->requestTeams(teams);
	}
	else
	{
		state_ = State::WaitingForRankings;
		blueAlliance()->requestRankings(dm_->evkey());
	}
}

void ImportMatchDataController::gotRankings()
{
	dm_->blockSignals(true);

	auto teams = blueAlliance()->getEngine().teams();
	for (auto team : teams)
	{
		QString key = team.second->key();

		if (!team.second->ranking().isEmpty())
			dm_->setTeamRanking(key, team.second->ranking());
	}

	dm_->blockSignals(false);

	state_ = State::Done;
	emit complete(false);

	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::MatchDataChanged);
	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::TeamDataChanged);
}