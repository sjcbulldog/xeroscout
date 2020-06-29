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

#include "ScoutingDataModel.h"
#include <QDialog>
#include <QTreeWidget>
#include <QDialogButtonBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SelectTeamsWidgetItem: public QTreeWidgetItem
			{
			public:
				SelectTeamsWidgetItem(QTreeWidget* w) : QTreeWidgetItem(w) {
				}

				virtual ~SelectTeamsWidgetItem() {
				}

				bool operator<(const QTreeWidgetItem& other) const {
					int mynum, othernum;

					if (text(1) == "-")
						mynum = 100 + text(2).toInt();
					else
						mynum = text(1).toInt();

					if (other.text(1) == "-")
						othernum = 100 + other.text(2).toInt();
					else
						othernum = other.text(1).toInt();

					return mynum > othernum;
				}
			};

			class SelectTeamsDialog : public QDialog
			{
			public:
				SelectTeamsDialog(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QStringList &keys);
				virtual ~SelectTeamsDialog();

				const QStringList& keys() {
					return keys_;
				}

			private:
				void populateTeams();
				void itemChanged(QTreeWidgetItem* item, int col);

			private:
				QStringList keys_;
				std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm_;

				QTreeWidget* tree_;
				QDialogButtonBox* box_;
			};
		}
	}
}

