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

#include "ApplicationController.h"
#include "BlueAlliance.h"
#include "ScoutingDataModel.h"
#include <QtNetwork/QNetworkAccessManager>

class ImportStatboticsController : public ApplicationController
{
public:
	ImportStatboticsController(std::shared_ptr<xero::ba::BlueAlliance> ba, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int year);
	virtual ~ImportStatboticsController();

	bool isDone() override;
	void run() override;

	virtual bool providesProgress() { return true; }
	virtual int percentDone();

private:
	void requestNextTeam();
	void finished();
	void sslError(QNetworkReply* reply, const QList<QSslError>& errors);

private:
	static constexpr const char * Server  = "https://api.statbotics.io/v3";

private:
	enum class State {
		Start,
		WaitingForData,
		Done,
		Error
	};

private:
	QNetworkAccessManager *netmgr_;
	State state_;
	QVector<int> teams_;
	QNetworkReply* reply_;
	int year_;
	int current_team_;
	int total_teams_;
	QMap<QString, QJsonDocument> data_;
};

