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
		picks_ = "<p>cannot open temporary file '" + file.fileName() + "' for the event information";
		caps_ = picks_;
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

void PickListGenerator::finished(int exitcode, QProcess::ExitStatus status)
{
	while (process_->canReadLine())
	{
		QString line = process_->readLine();
		emit logMessage(line);
	}

	if (status == QProcess::ExitStatus::NormalExit)
	{
		if (exitcode == 0)
		{
			QString htmlfile = dir_->path() + "/picklist.html";
			QFile read(htmlfile);
			read.open(QIODevice::ReadOnly);
			QTextStream strm(&read);
			picks_ = strm.readAll();

			htmlfile = dir_->path() + "/robot_capabilities.html";
			QFile read2(htmlfile);
			read2.open(QIODevice::ReadOnly);
			QTextStream strm2(&read2);
			caps_ = strm2.readAll();

			QString datafile = dir_->path() + "/picklist.csv";
			CsvReader reader(true);
			if (!reader.readFile(std::filesystem::path(datafile.toStdString()), CsvReader::HeaderType::Headers))
			{
				picks_ = "<p>error reading CSV data file";
			}
			else
			{
				dm_->clearPickList();
				for (size_t row = 0; row < reader.rowCount(); row++)
				{
					DataElem data = reader.get(row, 1);
					if (!std::holds_alternative<double>(data))
					{
						picks_ = "<p>bad data read from picklist CSV file";
						break;
					}

					int team = static_cast<int>(std::get<double>(data));

					data = reader.get(row, 2);
					if (!std::holds_alternative<double>(data))
					{
						picks_ = "<p>bad data read from picklist CSV file";
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
							picks_ = "<p>bad data read from picklist CSV file";
							break;
						}

						team = static_cast<int>(std::get<double>(data));

						data = reader.get(row, col + 1 );
						if (!std::holds_alternative<double>(data))
						{
							picks_ = "<p>bad data read from picklist CSV file";
							break;
						}

						score = std::get<double>(data);

						entry.addThird(team, score);

						col += 2;
					}

					dm_->addPickListEntry(entry);
				}

				dm_->resetPicklist();
			}

		}
		else
		{
			picks_ = "<p>analysis program exited with non-zero exit code, " + QString::number(exitcode) + "</p>";
		}
	}
	else
	{
		picks_ = "<p>analysis program crashed</p>";
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
		picks_ = "<p>The executable file '" + binfile + "' is not present";
		done_ = true;
		return;
	}

	dir_ = new QTemporaryDir();
	if (!dir_->isValid())
	{
		picks_ = "<p>cannot create a temporary directory to run the application";
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

