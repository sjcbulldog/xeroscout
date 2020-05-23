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
					bool ret;

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

