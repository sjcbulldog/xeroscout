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
