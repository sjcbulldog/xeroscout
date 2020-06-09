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

#include "ManagerBase.h"
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDirIterator>
#include <QDebug>
#include <QCoreApplication>
#include <stdexcept>

using namespace xero::paths;

ManagerBase::ManagerBase()
{
}

ManagerBase::~ManagerBase()
{
}

void ManagerBase::dumpSearchPath(const char* name)
{
	int index = 0;
	qDebug() << name << " search path";
	qDebug() << "-------------------------------------------";
	for (auto d : dirs_)
		qDebug() << index++ << ": " << d.c_str();
	qDebug() << "-------------------------------------------";
}

bool ManagerBase::initialize()
{
	for (const std::string& path : dirs_)
	{
		processPath(path);
	}

	if (!checkCount())
		return false;

	return true;
}

bool ManagerBase::processPath(const std::string& path)
{
	QStringList filters;
	filters.push_back("*.json");
	QDirIterator iter(path.c_str(), filters);

	while (iter.hasNext())
	{
		QFile f(iter.next());
		try {
		    qDebug() << "Processing JSON file '" << f.fileName() << "'" ;
			if (!processJSONFile(f))
			{
			    qDebug() << "   - failed to process file" ;
			    continue ;
			}
		}
		catch (const std::runtime_error& )
		{
		    qDebug() << "   - failed to process file" ;
		    continue ;
		}
	}

	return true;
}

bool ManagerBase::getJSONStringValue(QFile &file, QJsonDocument& doc, const char* name, std::string& value)
{
	const QJsonValue title = doc[name];
	if (title.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name 
			<< "' at the top level";
		return false;
	}

	if (!title.isString())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name 
			<< "' at the top level, but it is not a string";
		return false;
	}

	value = title.toString().toStdString();
	return true;
}

bool ManagerBase::getJsonVersionValue(QFile& file, QJsonDocument& doc, const char* name, QVersionNumber& value)
{
	const QJsonValue versionstruct = doc[name];
	if (versionstruct.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	if (!versionstruct.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it is not an object";
		return false;
	}

	QJsonObject obj = versionstruct.toObject();
	if (!obj.contains("major") || !obj["major"].isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it does not contain a valid 'major' field, should be number";
		return false;
	}
	int major = static_cast<int>(obj["major"].toDouble() + 0.5);

	if (!obj.contains("minor") || !obj["minor"].isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it does not contain a valid 'minor' field, should be number";
		return false;
	}
	int minor = static_cast<int>(obj["minor"].toDouble() + 0.5);

	if (!obj.contains("micro") || !obj["micro"].isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it does not contain a valid 'micro' field, should be number";
		return false;
	}
	int micro = static_cast<int>(obj["micro"].toDouble() + 0.5);

	value = QVersionNumber(major, minor, micro);
	return true;
}

bool ManagerBase::getJSONStringValue(QFile& file, QJsonObject& obj, const char* name, std::string& value)
{
	const QJsonValue v = obj[name];
	if (v.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	if (!v.isString())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it is not a string";
		return false;
	}

	value = v.toString().toStdString();
	return true;
}

bool ManagerBase::getJSONDoubleValue(QFile& file, QJsonDocument& doc, const char* name, double& value)
{
	const QJsonValue title = doc[name];
	if (title.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	if (!title.isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it is not a double";
		return false;
	}

	value = title.toDouble();
	return true;
}

bool ManagerBase::getJSONDoubleValue(QFile& file, QJsonObject& obj, const char* name, double& value)
{
	const QJsonValue title = obj[name];
	if (title.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	if (!title.isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it is not a double";
		return false;
	}

	value = title.toDouble();
	return true;
}

bool ManagerBase::getJSONIntegerValue(QFile& file, QJsonDocument& doc, const char* name, int& value)
{
	const QJsonValue title = doc[name];
	if (title.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	if (!title.isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' at the top level, but it is not a double";
		return false;
	}

	value = static_cast<int>(title.toDouble());
	return true;
}

bool ManagerBase::getJSONPointValue(QFile& file, QJsonDocument& doc, const char* name, Translation2d& t2d)
{
	const QJsonValue point = doc[name];

	if (point.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << name
			<< "' at the top level";
		return false;
	}

	return getJSONPointValue(file, point, name, t2d);
}

bool ManagerBase::getJSONPointValue(QFile &file, const QJsonValue &point, const char *name, Translation2d &t2d)
{
	if (!point.isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' but it is not a array";
		return false;
	}

	QJsonArray arr = point.toArray();
	if (arr.count() != 2)
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name 
			<< "' but the array did not have two entries";
		return false;
	}

	if (!arr[0].isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' but the first entry was not a double";
		return false;
	}

	if (!arr[1].isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << name
			<< "' but the second entry was not a double";
		return false;
	}

	t2d = Translation2d(arr[0].toDouble(), arr[1].toDouble());

	return true;
}

void ManagerBase::copyDefaults(const std::string& subdir)
{
	if (default_dir_.length() == 0)
		return;

	QString exedir = QCoreApplication::applicationDirPath();
	QString src = exedir + "/" + QString(subdir.c_str());

	QDir srcdir(src);
	QDir destdir(default_dir_.c_str());

	if (!destdir.exists())
	{
		if (!destdir.mkpath(default_dir_.c_str()))
			return;
	}

	QStringList list = srcdir.entryList(QDir::Filter::Files);
	for (QString item : list)
	{
		QString srcfile = srcdir.absoluteFilePath(item);
		QString destfile = destdir.absoluteFilePath(item);

		if (!QFile::exists(destfile))
			QFile::copy(srcfile, destfile);
	}
}
