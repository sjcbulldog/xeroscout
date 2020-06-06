#include "PickListGenerator.h"
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QProcess>
#include <QCoreApplication>

using namespace xero::scouting::datamodel;

PickListGenerator::PickListGenerator(int team, std::shared_ptr<ScoutingDataModel> dm, int year)
{
	team_ = team;
	dm_ = dm;
	year_ = year;

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


/////////////////////////////////////////////////////////////////////////////////////////////
//
// This is year specific, fix this
//
/////////////////////////////////////////////////////////////////////////////////////////////


bool PickListGenerator::didClimb(ConstScoutingDataMapPtr data)
{
	static char* pos[] = {
		"endgame__pneg3",
		"endgame__pneg2",
		"endgame__pneg1",
		"endgame__pzero",
		"endgame__ppos1",
		"endgame__ppos2",
		"endgame__ppos3"
	};

	auto it = data->find("endgame__position");
	for (const char* name : pos)
	{
		if (it->second.toString() == name)
			return true;
	}

	return false;
}

void PickListGenerator::oneTeam(QTextStream& strm, std::shared_ptr<const DataModelMatch> m, Alliance c, int slot)
{
	auto tkey = m->team(c, slot);
	auto team = dm_->findTeamByKey(tkey);
	auto data = m->scoutingData(c, slot);

	strm << team->number();
	strm << "," << m->match();
	strm << "," << toString(c);

	auto it = data->find("auto__line");
	strm << "," << it->second.toInt();

	it = data->find("auto__lowgoal");
	strm << "," << it->second.toInt();

	it = data->find("auto__highgoal");
	strm << "," << it->second.toInt();

	it = data->find("teleop__lowgoal");
	strm << "," << it->second.toInt();

	it = data->find("teleop__highgoal");
	strm << "," << it->second.toInt();

	it = data->find("teleop__spin");
	strm << "," << it->second.toInt();

	it = data->find("teleop__position");
	strm << "," << it->second.toInt();

	if (didClimb(data))
		strm << ",1";
	else
		strm << ",0";

	it = data->find("endgame__number_assisted");
	strm << "," << it->second.toInt();

	it = data->find("endgame__was_assisted");
	strm << "," << it->second.toInt();

	it = data->find("endgame__position");
	if (it->second.toString() == "parked")
		strm << ",1";
	else
		strm << ",0";

	it = data->find("endgame__level");
	strm << "," << it->second.toInt();

	strm << "\n";
}

QString PickListGenerator::genInputFile()
{
	QFile file(dir_->filePath("input.csv"));
	if (!file.open(QIODevice::WriteOnly))
	{
		picks_ = "<p>cannot open temporary file '" + file.fileName() + "' for the event information";
		done_ = true;
		return "";
	}

	QTextStream strm(&file);
	strm << "team,match,alliance,auto_line,auto_low,auto_high,tele_low,tele_high,wheel_spin,wheel_color,climbed,climb_assists,climb_was_assisted,park,balanced\n";

	for (auto m : dm_->matches())
	{
		if (m->compType() != "qm")
			continue;

		Alliance c = Alliance::Red;
		for (int slot = 1; slot <= 3; slot++)
		{
			oneTeam(strm, m, c, slot);
		}

		c = Alliance::Blue;
		for (int slot = 1; slot <= 3; slot++)
		{
			oneTeam(strm, m, c, slot);
		}
	}

	strm.flush();
	file.close();

	return file.fileName();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// End year specific code
//
/////////////////////////////////////////////////////////////////////////////////////////////

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

	QString binfile = QCoreApplication::applicationDirPath() + "/analysis" + QString::number(year_) + ".exe";
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

#ifdef _DEBUG
	QString dest = "C:/cygwin64/home/ButchGriffin/projects/scouting/input.csv";
	QFile::copy(filename, dest);
#endif

	emit logMessage("Generated Input File: " + filename);
	emit logMessage("Directory: " + dir_->path());

	QStringList args;
	args << "--team" << "1425" << "--file" << filename;
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

