#pragma once
#include "ApplicationController.h"
#include "ScoutingDataModel.h"
#include "ScoutingDataMap.h"
#include <QStringList>
#include <map>

class ImportMatchDataController : public ApplicationController
{
public:
	ImportMatchDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm,
		int maxmatch = std::numeric_limits<int>::max());
	virtual ~ImportMatchDataController();

	bool isDone() override;
	void run() override;

private:
	enum class State
	{
		Start,
		Done,
		Error,
		WaitingForMatches,
		WaitingForDetail,
		WaitingForTeams,
		WaitingForRankings
	};

private:
	void breakOutBAData(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, xero::scouting::datamodel::Alliance c, xero::scouting::datamodel::ScoutingDataMapPtr data);
	void breakoutBlueAlliancePerRobotData(std::map<QString, std::pair<xero::scouting::datamodel::ScoutingDataMapPtr, xero::scouting::datamodel::ScoutingDataMapPtr>>& data);
	void gotDetail();
	void gotRankings();
	void processFieldDesc(xero::scouting::datamodel::ScoutingDataMapPtr data);
	void setExtraFields();

	std::shared_ptr<xero::scouting::datamodel::FieldDesc> findField(const QString& name) {
		for (auto f : fields_)
			if (f->name() == name)
				return f;

		return nullptr;
	}

private:
	State state_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	std::list<std::shared_ptr<xero::scouting::datamodel::FieldDesc>> fields_;
	std::map<QString, QStringList> strings_;
	int maxmatch_;
};

