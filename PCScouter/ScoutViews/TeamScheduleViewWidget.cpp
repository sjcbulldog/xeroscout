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

#include "TeamScheduleViewWidget.h"

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			TeamScheduleViewWidget::TeamScheduleViewWidget(const QString &tablet, QWidget* parent) : QTreeWidget(parent), ViewBase("TeamScheduleViewWidget")
			{
				QStringList headers = { "Number", "Scouted", "Tablet", "Name" };

				setColumnCount(headers.size());
				setHeaderLabels(headers);

				setTablet(tablet);
			}

			TeamScheduleViewWidget::~TeamScheduleViewWidget()
			{
			}

			void TeamScheduleViewWidget::setScoutingField(const QString& key, bool value)
			{
				int col = 1;

				for (int i = 0; i < topLevelItemCount(); i++)
				{
					QTreeWidgetItem* item = topLevelItem(i);
					QString itemkey = item->data(0, Qt::UserRole).toString();

					if (itemkey == key)
					{
						item->setTextAlignment(col, Qt::AlignHCenter);
						if (value)
						{
							item->setText(col, "Y");
							item->setBackground(col, QBrush(QColor(0, 255, 0, 255)));
						}
						else
						{
							item->setText(col, "N");
							item->setBackground(col, QBrush(QColor(255, 0, 0, 255)));
						}
					}
				}
			}

			void TeamScheduleViewWidget::setData(std::list<std::shared_ptr<const DataModelTeam>> teams)
			{
				teams_ = teams;
				refreshView();
			}

			void TeamScheduleViewWidget::refreshView()
			{
				clearView();

				for (auto team : teams_) {
					int index = 0;

					QTreeWidgetItem* item = new TeamScheduleViewWidgetItem(this);
					item->setText(index++, QString::number(team->number()));

					if (tablet_.length() == 0 || tablet_ == team->tablet())
					{
						item->setTextAlignment(index, Qt::AlignHCenter);
						if (team->teamScoutingData() != nullptr)
						{
							item->setText(index, "Y");
							item->setBackground(index, QBrush(QColor(0, 255, 0, 255)));
						}
						else
						{
							item->setText(index, "N");
							item->setBackground(index, QBrush(QColor(255, 0, 0, 255)));
						}
					}
					else
					{
						item->setText(index, "");
					}
					index++;

					item->setText(index++, team->tablet());
					item->setText(index++, team->nick());

					item->setData(0, Qt::UserRole, QVariant(team->key()));
					addTopLevelItem(item);
				}

				resizeColumnToContents(0);
				resizeColumnToContents(1);
				resizeColumnToContents(2);
				resizeColumnToContents(3);

				sortByColumn(0, Qt::AscendingOrder);
			}


			void TeamScheduleViewWidget::clearView()
			{
				while (topLevelItemCount() > 0) {
					auto item = takeTopLevelItem(0);
					delete item;
				}
			}
		}
	}
}
