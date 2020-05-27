#pragma once

#include "BlueAlliance.h"
#include "Event.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include "ApplicationController.h"
#include <QJsonObject>
#include <QStringList>
#include <QString>
#include <memory>

class KPIController : public ApplicationController
{
public:
	KPIController(std::shared_ptr<xero::ba::BlueAlliance> ba, int year, const QStringList& teams,
		const QString& evkey, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team,
		std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match);
	virtual ~KPIController();

	void run();

	bool isDone() {
		return state_ == State::Done;
	}

private:
	enum class State
	{
		Done,
		Start,
		WaitingOnEvents,
		WaitingOnTeams,
		WaitingOnTeamEvents,
		WaitingOnMatches,
		WaitingOnMatchDetail,
	};

private:
	void getEvents();
	void getTeams();
	void getTeamEvents();

	void gotTeamEvents();
	void gotMatches();

	void computeKPI();
private:
	State state_;
	QStringList teams_;
	int year_;
	QStringList evlist_;
	QString evkey_;
	int index_ ;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> team_;
	std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> match_;
};

