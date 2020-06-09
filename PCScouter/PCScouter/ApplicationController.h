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

