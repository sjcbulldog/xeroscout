//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "PCScouter.h"
#include "TestDataInjector.h"
#include <QApplication>
#include <QMessageBox>
#include <SingleInstanceGuard.h>

int main(int argc, char *argv[])
{
	SingleInstanceGuard guard("SingleInstance-PCScouter");
	if (!guard.tryToRun()) {
		return 0;
	}

	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("PCScouter");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication a(argc, argv);

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
		else
		{
			QMessageBox::critical(nullptr, "Error", "invalid command line argument '" + arg + "'");
			return 1;
		}
	}

	try {
		PCScouter w;
		w.show();
		return a.exec();
	}
	catch (const std::exception &ex) {
		return 1;
	}

	return 0;
}
