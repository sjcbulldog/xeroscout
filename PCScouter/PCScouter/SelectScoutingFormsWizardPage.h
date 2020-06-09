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

#pragma once
#include "NewEventBlueAllianceWizard.h"
#include <QWizard>
#include <QLabel>
#include <QSettings>

class SelectScoutingFormsWizardPage : public QWizardPage
{
public:
	SelectScoutingFormsWizardPage(NewEventBlueAllianceWizard::PropertyMap &props);
	virtual ~SelectScoutingFormsWizardPage();

	bool isComplete() const override;

public:
	static constexpr const char* PitScoutFieldName = "pitscout";
	static constexpr const char* MatchScoutFieldName = "matchscout";

protected:
	void resizeEvent(QResizeEvent* ev);

private:
	void selectPitScoutForm();
	void selectMatchScoutForm();

	void setLabelText(QLabel* label, const QString& str);

private:
	QLabel* pit_scouting_form_;
	QLabel* match_scouting_form_;
	QString pit_filename_;
	QString match_filename_;

	QSettings settings_;

	NewEventBlueAllianceWizard::PropertyMap& props_;
};

