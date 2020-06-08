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

#pragma once

#include "ScoutingDataModel.h"
#include "BlueAlliance.h"
#include <QObject>
#include <memory>

class ApplicationController : public QObject
{
	friend class PCScouter;

	Q_OBJECT

public:
	ApplicationController(std::shared_ptr<xero::ba::BlueAlliance> ba, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm);
	virtual ~ApplicationController();

	bool isDisplayInitialized() { return display_initialized_; }
	void setDisplayInitialized() { display_initialized_ = true; }

	virtual bool isDone() = 0;
	virtual void run() = 0;

	virtual bool shouldDisableApp() { return true; }

	virtual bool providesProgress() { return false; }
	virtual int percentDone() { return 0; }

signals:
	void errorMessage(const QString& msg);
	void logMessage(const QString& msg);
	void complete(bool err);

protected:
	std::shared_ptr<xero::ba::BlueAlliance> blueAlliance() {
		return ba_;
	}

	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dataModel() {
		return dm_;
	}

	void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm) {
		dm_ = dm;
	}

private:
	void restoreBlueAllianceData();

private:
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	std::shared_ptr<xero::ba::BlueAlliance> ba_;
	bool display_initialized_;
};

