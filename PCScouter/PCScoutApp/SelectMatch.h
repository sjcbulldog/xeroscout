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
