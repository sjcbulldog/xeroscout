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

#include "PickListGenerator.h"
#include "CsvReader.h"
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QProcess>
#include <QCoreApplication>

using namespace xero::scouting::datamodel;

PickListGenerator::PickListGenerator(int team, std::shared_ptr<ScoutingDataModel> dm, const QString &name)
{
	team_ = team;
	dm_ = dm;
	picklist_pgm_name_ = name;

	dir_ = nullptr;
	process_ = nullptr;
	done_ = false;
}

PickListGenerator::~PickListGenerator()
{
	if (process_ != nullptr)
		delete process_;

	if (dir_ != nullptr)
		delete dir_;
}

QString PickListGenerator::genQuery()
{
	QString query = "select ";
	bool first = true;

	for (auto field : dm_->pickListTranslator()->fields())
	{
		if (!first)
			query += ",";

		query += field.second;
		first = false;
	}
	query += " from matches where Type='qm'";

	return query;
}

QString PickListGenerator::genInputFile()
{
	QFile file(dir_->filePath("input.csv"));
	if (!file.open(QIODevice::WriteOnly))
	{
		picks_error_msg_ =  "<p>cannot open temporary file '" + file.fileName() + "' for the event information";
		caps_error_msg_ = picks_error_msg_;
		done_ = true;
		return "";
	}

	QString query = genQuery();
	ScoutingDataSet ds("$picklist");
	QString error;
	if (!dm_->createCustomDataSet(ds, query, error))
	{
		emit logMessage("Query Error: " + error);
		return "";
	}

	QTextStream strm(&file);
	bool first = true;
	for (auto field : dm_->pickListTranslator()->fields())
	{
		if (!first)
			strm << ",";

		strm << field.first;
		first = false;
	}
	strm << "\n";

	for (int row = 0; row < ds.rowCount(); row++)
	{
		first = true;
		for (int col = 0; col < ds.columnCount(); col++)
		{
			if (!first)
				strm << ",";

			auto hdr = ds.colHeader(col);
			if (hdr->type() == FieldDesc::Type::String)
			{
				strm << ds.get(row, col).toString();
			}
			else
			{
				strm << ds.get(row, col).toInt();
			}
			first = false;
		}
		strm << "\n";
	}

	strm.flush();
	file.close();

	return file.fileName();
}

void PickListGenerator::started()
{
}

bool PickListGenerator::readPickList(const QString& datafile)
{
	CsvReader reader(true);
	if (!reader.readFile(std::filesystem::path(datafile.toStdString()), CsvReader::HeaderType::Headers))
	{
		picks_error_msg_ = "cannot open file '" + datafile + "' for reading";
		return false;
	}

	if (reader.rowCount() == 0)
	{
		picks_error_msg_ = "no data found in file '" + datafile + "' for reading";
		return false;
	}

	dm_->blockSignals(true);
	dm_->clearPickList();
	for (size_t row = 0; row < reader.rowCount(); row++)
	{
		DataElem data = reader.get(row, 1);
		if (!std::holds_alternative<double>(data))
		{
			picks_error_msg_ = "bad data read from picklist CSV file";
			break;
		}

		int team = static_cast<int>(std::get<double>(data));

		data = reader.get(row, 2);
		if (!std::holds_alternative<double>(data))
		{
			picks_error_msg_ = "bad data read from picklist CSV file";
			break;
		}

		double score = std::get<double>(data);

		PickListEntry entry(team, score);

		size_t col = 3;
		while (col + 1 < reader.colCount())
		{
			data = reader.get(row, col);
			if (!std::holds_alternative<double>(data))
			{
				picks_error_msg_ = "bad data read from picklist CSV file";
				break;
			}

			team = static_cast<int>(std::get<double>(data));

			data = reader.get(row, col + 1);
			if (!std::holds_alternative<double>(data))
			{
				picks_error_msg_ = "bad data read from picklist CSV file";
				break;
			}

			score = std::get<double>(data);

			entry.addThird(team, score);

			col += 2;
		}

		dm_->addPickListEntry(entry);
	}
	dm_->resetPicklist();
	dm_->blockSignals(false);
	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::PickListChanged);

	return true;
}

bool PickListGenerator::readRobotCapabilities(const QString& datafile)
{
	CsvReader reader(true);
	if (!reader.readFile(std::filesystem::path(datafile.toStdString()), CsvReader::HeaderType::Headers))
	{
		caps_error_msg_ = "cannot open file '" + datafile + "' for reading";
		return false;
	}

	if (reader.rowCount() == 0)
	{
		caps_error_msg_ = "no data found in file '" + datafile + "' for reading";
		return false;
	}

	//
	// First extract the types for each column
	//
	std::vector<bool> types;
	for (int col = 0; col < reader.colCount(); col++)
	{
		auto value = reader.get(0, col);
		if (std::holds_alternative<double>(value))
		{
			types.push_back(false);
		}
		else if (std::holds_alternative<std::string>(value))
		{
			types.push_back(true);
		}
		else
		{
			caps_error_msg_ = "bad CSV data in file '" + datafile + "'";
			return false;
		}
	}

	if (types[0])
	{
		caps_error_msg_ = "invalid CSV data in file '" + datafile + "' - expected team field in first column";
		return false;
	}

	dm_->blockSignals(true);
	for (int row = 0; row < reader.rowCount(); row++)
	{
		auto value = reader.get(row, 0);
		if (!std::holds_alternative<double>(value))
		{
			caps_error_msg_ = "invalid CSV data in file '" + datafile + "'";
			caps_error_msg_ += ", row " + QString::number(row + 1);
			caps_error_msg_ += " - expected team field in first column";
			return false;
		}

		RobotCapabilities caps(static_cast<int>(std::get<double>(value) + 0.5));
		for (int col = 1; col < reader.colCount(); col++)
		{
			QString name = (reader.headers())[col].c_str();
			value = reader.get(row, col);

			if (types[col])
			{
				if (!std::holds_alternative<std::string>(value))
				{
					caps_error_msg_ = "invalid CSV data in file '" + datafile + "'";
					caps_error_msg_ += ", row " + QString::number(row + 1);
					caps_error_msg_ += ", col " + QString::number(col + 1);
					caps_error_msg_ += " - expected string representation of a distrubution";
					return false;
				}

				Distribution dist;

				if (!parseDist(dist, QString(std::get<std::string>(value).c_str())))
				{
					caps_error_msg_ = "invalid CSV data in file '" + datafile + "'";
					caps_error_msg_ += ", row " + QString::number(row + 1);
					caps_error_msg_ += ", col " + QString::number(col + 1);
					caps_error_msg_ += " - expected string representation of a distrubution";
					return false;
				}
				caps.addDistParam(name, dist);
			}
			else
			{
				if (!std::holds_alternative<double>(value))
				{
					caps_error_msg_ = "invalid CSV data in file '" + datafile + "'";
					caps_error_msg_ += ", row " + QString::number(row + 1);
					caps_error_msg_ += ", col " + QString::number(col + 1);
					caps_error_msg_ += " - expected floating point number";
					return false;
				}

				caps.addDoubleParam(name, std::get<double>(value));
			}
		}

		dm_->addRobotCapability(caps);
	}

	dm_->blockSignals(false);
	dm_->emitChangedSignal(ScoutingDataModel::ChangeType::PickListChanged);

	return true;
}

bool PickListGenerator::parseDist(Distribution& dist, const QString& str)
{
	QStringList list = str.split(',');
	if ((list.size() % 2) != 0)
		return false;

	int i = 0; 
	while (i < list.size())
	{
		int bucket = list[i].toInt();
		double value = list[i + 1].toDouble();

		dist.push_back(std::make_pair(bucket, value));
		i += 2;
	}

	return true;
}

void PickListGenerator::finished(int exitcode, QProcess::ExitStatus status)
{
	QString datafile;

	while (process_->canReadLine())
	{
		QString line = process_->readLine();
		emit logMessage(line);
	}

	if (status == QProcess::ExitStatus::NormalExit)
	{
		if (exitcode == 0)
		{
			datafile = dir_->path() + "/picklist.csv";
			readPickList(datafile);

			datafile = dir_->path() + "/robot_capabilities.csv";
			readRobotCapabilities(datafile);
		}
		else
		{
			picks_error_msg_ =  "<p>analysis program exited with non-zero exit code, " + QString::number(exitcode) + "</p>";
		}
	}
	else
	{
		picks_error_msg_ =  "<p>analysis program crashed</p>";
	}

	done_ = true;
}

void PickListGenerator::stateChanged(QProcess::ProcessState state)
{
}

void PickListGenerator::readReady()
{
	while (process_->canReadLine())
	{
		QString line = process_->readLine();
		line = line.trimmed();
		emit logMessage(line);
	}
}

void PickListGenerator::errorOccurred(QProcess::ProcessError err)
{
	qDebug() << "errorOccured";
}

void PickListGenerator::start()
{
	done_ = false;

	QString binfile = QCoreApplication::applicationDirPath() + "/" + picklist_pgm_name_;
	if (!QFile::exists(binfile))
	{
		picks_error_msg_ =  "<p>The executable file '" + binfile + "' is not present";
		done_ = true;
		return;
	}

	dir_ = new QTemporaryDir();
	if (!dir_->isValid())
	{
		picks_error_msg_ =  "<p>cannot create a temporary directory to run the application";
		done_ = true;
		return;
	}

	//
	// Generate the input file
	//
	QString filename = genInputFile();
	if (done_ == true)
		return;

	QString dest = "C:/cygwin64/home/ButchGriffin/projects/scouting/PCScouter/files/input.csv";
	QFile::copy(filename, dest);

	emit logMessage("Generated Input File: " + filename);
	emit logMessage("Directory: " + dir_->path());

	QStringList args;
	args << "1425" << filename;
	process_ = new QProcess();
	process_->setWorkingDirectory(dir_->path());

	connect(process_, &QProcess::started, this, &PickListGenerator::started);
	connect(process_, static_cast<void (QProcess::*)(int exitcode, QProcess::ExitStatus status)>(&QProcess::finished), this, &PickListGenerator::finished);
	connect(process_, &QProcess::stateChanged, this, &PickListGenerator::stateChanged);
	connect(process_, &QProcess::readyReadStandardOutput, this, &PickListGenerator::readReady);
	connect(process_, &QProcess::readyReadStandardError, this, &PickListGenerator::readReady);
	connect(process_, &QProcess::errorOccurred, this, &PickListGenerator::errorOccurred);

	process_->start(binfile, args);
}

