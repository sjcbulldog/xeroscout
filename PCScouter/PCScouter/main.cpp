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

#include "PCScouter.h"
#include "TestDataInjector.h"
#include <QApplication>
#include <QMessageBox>
#include <SingleInstanceGuard.h>

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	SingleInstanceGuard guard("SingleInstance-PCScouter");
	if (!guard.tryToRun()) {
		return 0;
	}
#endif

	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("PCScouter");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication a(argc, argv);

	bool coach = false;

	int i = 1;
	QStringList args = QCoreApplication::arguments();
	TestDataInjector& injector = TestDataInjector::getInstance();

	while (i < args.size()) {
		QString arg = args[i++];
		if (arg == "--inject")
		{
			if (i == args.size())
			{
				QMessageBox::critical(nullptr, "Error", "argument --inject requires an NAME=VALUE line following");
				return 1;
			}

			if (!injector.parseData(args[i++]))
			{
				QMessageBox::critical(nullptr, "Error", "argument --inject, invalid following argument, expected NAME=VALUE");
				return 1;
			}
		}
		else if (arg == "--coach")
		{
			coach = true;
		}
		else
		{
			QMessageBox::critical(nullptr, "Error", "invalid command line argument '" + arg + "'");
			return 1;
		}
	}

	try {
		PCScouter w(coach);
		w.show();
		return a.exec();
	}
	catch (const std::exception &ex) {
		return 1;
	}

	return 0;
}
