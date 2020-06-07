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

#pragma once

#include "ScoutingDataModel.h"
#include <QDialog>
#include "ui_SelectMatch.h"

class SelectMatch : public QDialog
{
	Q_OBJECT

public:
	SelectMatch(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, QWidget *parent = Q_NULLPTR);
	~SelectMatch();

	QString matchKey() const {
		return match_key_;
	}

	QString whichPlayer() const {
		return ui.scout_list_->currentItem()->data(Qt::UserRole).toString();
	}

private:
	void indexChanged(int index);
	void itemDouble(QListWidgetItem* item);

private:
	Ui::SelectMatch ui;
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;
	QString match_key_;
};
