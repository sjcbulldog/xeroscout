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
				"Source",
				"UUID",
				"When",
				"Teams",
				"Matches"
			};

			ChangeHistoryView::ChangeHistoryView(QWidget* parent) : QTreeWidget(parent), ViewBase("ChangeHistoryView")
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
					if (!rec.id().uid().isNull())
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
