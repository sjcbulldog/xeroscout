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

#include "ChangeHistoryView.h"
#include <QShowEvent>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			QStringList ChangeHistoryView::headers_ =
			{
				"Tablet",
				"UUID",
				"When",
				"Teams",
				"Matches"
			};

			ChangeHistoryView::ChangeHistoryView(QWidget* parent) : QTreeWidget(parent)
			{
				setColumnCount(5);
				setHeaderLabels(headers_);
			}

			ChangeHistoryView::~ChangeHistoryView()
			{
			}

			void ChangeHistoryView::refreshView()
			{
				auto dm = dataModel();
				if (dm == nullptr)
					return;

				clear();

				for (const SyncHistoryRecord& rec : dm->history())
				{
					QTreeWidgetItem* item = new QTreeWidgetItem(this);

					item->setText(0, rec.id().name());
					item->setText(1, rec.id().uid().toString());
					item->setText(2, rec.when().toString());

					QString msg;
					for (int i = 0; i < rec.pits().size(); i++)
					{
						if (i != 0)
							msg += ", ";
						msg += rec.pits().at(i);
					}
					item->setText(3, msg);

					msg.clear();
					for (int i = 0; i < rec.matches().size(); i++)
					{
						if (i != 0)
							msg += ", ";
						msg += rec.matches().at(i);
					}
					item->setText(4, msg);

					addTopLevelItem(item);
				}

				for (int i = 0; i < columnCount(); i++)
				{
					resizeColumnToContents(i);
				}
			}

			void ChangeHistoryView::showEvent(QShowEvent* ev)
			{
				if (topLevelItemCount() == 0)
					refreshView();
			}
		}
	}
}
