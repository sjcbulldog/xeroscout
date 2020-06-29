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

#include "SelectScoutingFormsWizardPage.h"
#include "NewEventBlueAllianceWizard.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include <QVariant>

SelectScoutingFormsWizardPage::SelectScoutingFormsWizardPage(NewEventBlueAllianceWizard::PropertyMap &props) :props_(props)
{
	QLabel* l;
	QWidget* row;
	QPushButton* button;
	QHBoxLayout* rowlayout;


	setTitle("Select Forms");

	QVBoxLayout* layout = new QVBoxLayout();

	// Pit scouting row
	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);

	l = new QLabel(row);
	l->setText("Team Scouting Form: ");
	rowlayout->addWidget(l);

	pit_scouting_form_ = new QLabel(row);
	pit_scouting_form_->setText("NONE");
	rowlayout->addWidget(pit_scouting_form_, 100);

	button = new QPushButton(row);
	button->setText("...");
	rowlayout->addWidget(button);
	(void)connect(button, &QPushButton::pressed, this, &SelectScoutingFormsWizardPage::selectPitScoutForm);

	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);

	// Match scouting row
	l = new QLabel(row);
	l->setText("Match Scouting Form: ");
	rowlayout->addWidget(l);

	match_scouting_form_ = new QLabel(row);
	match_scouting_form_->setText("NONE");
	rowlayout->addWidget(match_scouting_form_, 100);

	button = new QPushButton(row);
	button->setText("...");
	rowlayout->addWidget(button);
	(void)connect(button, &QPushButton::pressed, this, &SelectScoutingFormsWizardPage::selectMatchScoutForm);

	setLayout(layout);
}

SelectScoutingFormsWizardPage::~SelectScoutingFormsWizardPage()
{
}

void SelectScoutingFormsWizardPage::initializePage()
{
	QWizardPage::initializePage();

	NewEventBlueAllianceWizard* w = dynamic_cast<NewEventBlueAllianceWizard*>(wizard());
	w->setTitle();
}

void SelectScoutingFormsWizardPage::resizeEvent(QResizeEvent* ev)
{
	setLabelText(pit_scouting_form_, pit_filename_);
	setLabelText(match_scouting_form_, match_filename_);
}

void SelectScoutingFormsWizardPage::setLabelText(QLabel* label, const QString& str)
{
	QFontMetrics fm(label->font());
	QString txt = fm.elidedText(str, Qt::TextElideMode::ElideLeft, pit_scouting_form_->width());
	label->setText(txt);
}

void SelectScoutingFormsWizardPage::selectPitScoutForm()
{
	QString dir;

	if (settings_.contains("FormsDir"))
		dir = settings_.value("FormsDir").toString();

	QString filename = QFileDialog::getOpenFileName(this, "Select Pit Scouting Form", dir, "JSON Files (*.json);;All Files (*.*)");
	props_[PitScoutFieldName] = QVariant(filename);

	pit_filename_ = filename;
	setLabelText(pit_scouting_form_, pit_filename_);

	QFileInfo file(filename);
	dir = file.absoluteDir().absolutePath();
	settings_.setValue("FormsDir", dir);

	emit completeChanged();
}

void SelectScoutingFormsWizardPage::selectMatchScoutForm()
{
	QString dir;

	if (settings_.contains("FormsDir"))
		dir = settings_.value("FormsDir").toString();

	QString filename = QFileDialog::getOpenFileName(this, "Select Match Scouting Form", dir, "JSON Files (*.json);;All Files (*.*)");
	props_[MatchScoutFieldName] = QVariant(filename);

	match_filename_ = filename;
	setLabelText(match_scouting_form_, match_filename_);

	QFileInfo file(filename);
	dir = file.absoluteDir().absolutePath();
	settings_.setValue("FormsDir", dir);

	emit completeChanged();
}

bool SelectScoutingFormsWizardPage::isComplete() const
{
	return props_.find(PitScoutFieldName) != props_.end() && props_.find(MatchScoutFieldName) != props_.end();
}
