//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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



#include "AboutDialog.h"
#include "build.h"
#include <QCoreApplication>
#include <QTextCharFormat>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define BUILDVERSION STR(XERO_MAJOR_VERSION) "." STR(XERO_MINOR_VERSION) "." STR(XERO_MICRO_VERSION) "." STR(XERO_BUILD_VERSION)

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/ErrorCodeXero.png";
	QPixmap image(imagepath);

	ui.setupUi(this);
	ui.image_->setPixmap(image);
	ui.text_->setReadOnly(true);

	QFont font = ui.text_->font();
	font.setPointSize(16);
	QTextCharFormat fmt;

	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	QString title = "Xero Scouter ";
	title += STR(XERO_MAJOR_VERSION) "." STR(XERO_MINOR_VERSION) "." STR(XERO_MICRO_VERSION);
	ui.text_->append(title);

	QString buildno = QString("\r\nBuild ") + QString(BUILDVERSION) + QString("\r\n\r\n");
	font.setPointSize(8);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append(buildno);

	font.setPointSize(12);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Brought to you by FRC Team 1425\r\nError Code Xero\r\n\r\n");


	font.setPointSize(8);
	font.setUnderline(true);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Icons From the Noun Project");

	font.setPointSize(6);
	font.setUnderline(false);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Summary Report by DailyPM from the Noun Project");
	ui.text_->append("customize by Adrien Coquet from the Noun Project");
	ui.text_->append("database by Nancy  from the Noun Project");
	ui.text_->append("database by IcoMoon from the Noun Project");
	ui.text_->append("team by AdbA Icons from the Noun Project");
	ui.text_->append("form by Adrien Coquet from the Noun Project");
	ui.text_->append("schedule by Larea from the Noun Project");
	ui.text_->append("Robotics by Mohamed Mbarki from the Noun Project");
	ui.text_->append("inspection pit by Andrei Yushchenko from the Noun Project");


	font.setPointSize(8);
	font.setUnderline(true);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Icons From the FlatIcon");

	font.setPointSize(6);
	font.setUnderline(false);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Icon made by mangsaabguru from www.flaticon.com mangsaabguru");
	ui.text_->append("Icon made by Prosymbols from www.flaticon.com mangsaabguru");

	(void)connect(ui.buttons_, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
}
