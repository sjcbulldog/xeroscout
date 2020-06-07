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
#include <QObject>
#include <QProcess>
#include <QTemporaryDir>

class PickListGenerator : public QObject
{
	Q_OBJECT

public:
	PickListGenerator(int team_, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int year);
	virtual ~PickListGenerator();

	void start();

	bool isDone() {
		return done_;
	}
	QString picklist() {
		return picks_;
	}

	QString robotCapabilities() const {
		return caps_;
	}

signals:
	void logMessage(const QString& msg);

private:
	bool didClimb(xero::scouting::datamodel::ConstScoutingDataMapPtr data);
	void oneTeam(QTextStream& strm, std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, xero::scouting::datamodel::Alliance c, int slot);

	QString genInputFile();
	QString genQuery();

	void started();
	void finished(int exitcode, QProcess::ExitStatus status);
	void stateChanged(QProcess::ProcessState state);
	void readReady();
	void errorOccurred(QProcess::ProcessError err);

private:
	int year_;
	int team_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QProcess* process_;
	QString picks_;
	QString caps_;
	bool done_;
	QTemporaryDir* dir_;
};
