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
#include "AboutDialog.h"
#include "build.h"
#include <QCoreApplication>
#include <QTextCharFormat>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define BUILDVERSION STR(XERO_MAJOR_VERSION) "." STR(XERO_MINOR_VERSION) "." STR(XERO_MICRO_VERSION) "." STR(XERO_BUILD_VERSION)

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
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
	QString title = "Xero Scout Central ";
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
	ui.text_->append("Icons from https://icons8.com");

	font.setPointSize(6);
	font.setUnderline(false);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Form icon icon by Icons8");
	ui.text_->append("Form icon icon by Icons8");
	ui.text_->append("Low Connection icon icon by Icons8");
	ui.text_->append("Low Connection icon icon by Icons8");
	ui.text_->append("Data Sheet icon icon by Icons8");
	ui.text_->append("Data Sheet icon icon by Icons8");
	ui.text_->append("People icon icon by Icons8");
	ui.text_->append("R2-D2 icon icon by Icons8");
	ui.text_->append("Brief icon icon by Icons8");
	ui.text_->append("SQL icon icon by Icons8");
	ui.text_->append("Time Machine icon icon by Icons8");
	ui.text_->append("Map Pinpoint icon icon by Icons8");
	ui.text_->append("Tick Box icon icon by Icons8");
	ui.text_->append("Plus Math icon icon by Icons8");
	ui.text_->append("Question Mark icon icon by Icons8");
	ui.text_->append("");

	font.setPointSize(8);
	font.setUnderline(true);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Icons from https://www.flaticon.com");

	font.setPointSize(6);
	font.setUnderline(false);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Icon made by mangsaabguru");
	ui.text_->append("Icon made by Prosymbols");

	(void)connect(ui.buttons_, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
}
