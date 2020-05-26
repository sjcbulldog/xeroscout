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

#include "SelectScoutingFormsWizardPage.h"
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
	l->setText("Pit Scouting Form: ");
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
