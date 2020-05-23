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
			class SCOUTVIEWS_EXPORT PitScheduleViewWidgetItem : public QTreeWidgetItem
			{
			public:
				PitScheduleViewWidgetItem(QTreeWidget* w) : QTreeWidgetItem(w) {
				}

				virtual ~PitScheduleViewWidgetItem() {
				}

				bool operator<(const QTreeWidgetItem& other) const {
					int mynum = text(0).toInt();
					int othernum = other.text(0).toInt();

					return mynum > othernum;
				}
			};

			class SCOUTVIEWS_EXPORT PitScheduleViewWidget : public QTreeWidget, public ViewBase
			{
			public:
				PitScheduleViewWidget(const QString &tablet, QWidget* parent);
				virtual ~PitScheduleViewWidget();

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
