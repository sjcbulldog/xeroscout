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

#include "Translation2d.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QVersionNumber>
#include <cassert>

class ManagerBase
{
public:
	ManagerBase();
	virtual ~ManagerBase();

	const std::string& getDefaultDir() const {
		return default_dir_;
	}

	virtual void copyDefaults(const std::string& subdir);

	virtual bool initialize();

	void setDirectory(const std::string& dir) {
		dirs_.clear();
		dirs_.push_back(dir);
	}

	void addDirectory(const std::string& dir) {
		dirs_.push_back(dir);
	}

	void addDefaultDirectory(const std::string& dir) {
		addDirectory(dir);
		default_dir_ = dir;
	}


	void dumpSearchPath(const char* name);

protected:
	virtual bool processPath(const std::string& path);
	virtual bool processJSONFile(QFile& file) = 0;
	virtual bool checkCount() = 0;

	const std::string& getFirstDir() {
		assert(dirs_.size() > 0);
		return dirs_.front();
	}

	bool getJsonVersionValue(QFile& file, QJsonDocument& doc, const char* name, QVersionNumber& ver);
	bool getJSONStringValue(QFile &file, QJsonDocument& doc, const char* name, std::string& value);
	bool getJSONStringValue(QFile& file, QJsonObject& obj, const char* name, std::string& value);
	bool getJSONDoubleValue(QFile& file, QJsonDocument& doc, const char* name, double& value);
	bool getJSONDoubleValue(QFile& file, QJsonObject& doc, const char* name, double& value);
	bool getJSONIntegerValue(QFile& file, QJsonDocument& doc, const char* name, int& value);
	bool getJSONPointValue(QFile& file, const QJsonValue& value, const char *name, xero::paths::Translation2d& t2d);
	bool getJSONPointValue(QFile& file, QJsonDocument& doc, const char* name, xero::paths::Translation2d& t2d);

private:
	std::string default_dir_;
	std::list<std::string> dirs_;
};

