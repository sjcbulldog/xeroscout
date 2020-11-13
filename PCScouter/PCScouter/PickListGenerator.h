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
	PickListGenerator(int team_, std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QString &pgm);
	virtual ~PickListGenerator();

	void start();

	bool isDone() {
		return done_;
	}

	bool picklistError() {
		return picks_error_;
	}

	QString robotCapabilitiesError() const {
		return caps_error_;
	}

signals:
	void logMessage(const QString& msg);

private:
	QString genInputFile();
	QString genQuery();

	void started();
	void finished(int exitcode, QProcess::ExitStatus status);
	void stateChanged(QProcess::ProcessState state);
	void readReady();
	void errorOccurred(QProcess::ProcessError err);

	bool readPickList(const QString& filename);
	bool readRobotCapabilities(const QString& filename);

	bool parseDist(xero::scouting::datamodel::Distribution& dist, const QString& str);

private:
	int team_;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QProcess* process_;
	bool done_;
	QTemporaryDir* dir_;
	QString picklist_pgm_name_;

	bool picks_error_;
	QString picks_error_msg_;
	bool caps_error_;
	QString caps_error_msg_;
};
