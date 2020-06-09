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

#include "scoutviews_global.h"
#include "DataModelTeam.h"
#include "ViewBase.h"
#include <QTreeWidget>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT TeamScheduleViewWidgetItem : public QTreeWidgetItem
			{
			public:
				TeamScheduleViewWidgetItem(QTreeWidget* w) : QTreeWidgetItem(w) {
				}

				virtual ~TeamScheduleViewWidgetItem() {
				}

				bool operator<(const QTreeWidgetItem& other) const {
					int mynum = text(0).toInt();
					int othernum = other.text(0).toInt();

					return mynum > othernum;
				}
			};

			class SCOUTVIEWS_EXPORT TeamScheduleViewWidget : public QTreeWidget, public ViewBase
			{
			public:
				TeamScheduleViewWidget(const QString &tablet, QWidget* parent);
				virtual ~TeamScheduleViewWidget();

				void setData(std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelTeam>> teams);
				void clearView();
				void refreshView();

				void setTablet(const QString& t) override {
					tablet_ = t;
					refreshView();

					if (tablet_.length() > 0) {
						QFont f = font();
						f.setPointSizeF(18.0);
						setFont(f);
					}
				}

				void setScoutingField(const QString& key, bool value);

			private:
				QString tablet_;
				std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelTeam>> teams_;
			};
		}
	}
}
