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
							item->setBackgroundColor(col, QColor(0, 255, 0, 255));
						}
						else
						{
							item->setText(col, "N");
							item->setBackgroundColor(col, QColor(255, 0, 0, 255));
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
							item->setBackgroundColor(index, QColor(0, 255, 0, 255));
						}
						else
						{
							item->setText(index, "N");
							item->setBackgroundColor(index, QColor(255, 0, 0, 255));
						}
					}
					else
					{
						item->setText(index, "");
					}
					index++;

					item->setText(index++, team->tablet());
					item->setText(index++, team->name());

					item->setData(0, Qt::UserRole, QVariant(team->key()));
					addTopLevelItem(item);
				}

				resizeColumnToContents(0);
				resizeColumnToContents(1);
				resizeColumnToContents(2);
				resizeColumnToContents(3);

				sortByColumn(0);
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
