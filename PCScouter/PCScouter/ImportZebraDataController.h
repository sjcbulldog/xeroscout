#pragma once
#include "ApplicationController.h"
#include "ScoutingDataModel.h"

class ImportZebraDataController : public ApplicationController
{
public:
	ImportZebraDataController(std::shared_ptr<xero::ba::BlueAlliance> ba,
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	virtual ~ImportZebraDataController();

	bool isDone() override;
	void run() override;

private:
	enum class State
	{
		Start,
		Done,
		Error,
		WaitingForMatches,
		WaitingForZebra
	};

private:
	void gotZebra();

private:
	State state_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
};

