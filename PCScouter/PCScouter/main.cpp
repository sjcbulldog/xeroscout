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

	bool central = false;
	QString name;

	int i = 0;
	QStringList args = QCoreApplication::arguments();
	while (i < args.size()) {
		if (args[i] == "--central")
			central = true;
		else
			name = args[i];

		i++;
	}

	try {
		PCScouter w;
		w.show();
		return a.exec();
	}
	catch (...) {
	}

	return 1;
}
