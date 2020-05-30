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
#include "ScoutingForm.h"
#include "ScoutingDataModel.h"
#include "ViewBase.h"
#include "GameFieldManager.h"
#include "ImageManager.h"
#include <QStackedWidget>
#include <QTreeWidget>
#include <map>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT DocumentView : public QStackedWidget
			{
				Q_OBJECT

			public:
				enum class ViewType : int
				{
					NoModelView = 0,
					TeamScoutingFormView = 1,
					MatchScoutingFormViewRed = 2,
					MatchScoutingFormViewBlue = 3,
					PitView = 4,
					MatchView = 5,
					MatchDataSet = 6,
					TeamDataSet = 7,
					CustomDataSet = 8,
					TeamReport = 9,
					AllTeamReport = 10,
					HistoryView = 11,
					MergeListView = 12,
					ZebraDataView = 13,
					MatchGraphView = 14,
					AllianceGraphView = 15,
					FirstFormView = 16
				};

			public:
				DocumentView(int year, const QString &tablet, QWidget* parent = Q_NULLPTR);
				~DocumentView();

				void setTablet(const QString& tablet) {
					for (int i = 0; i < count(); i++)
					{
						QWidget* w = widget(i);
						ViewBase* vb = dynamic_cast<ViewBase*>(w);
						if (vb != nullptr)
							vb->setTablet(tablet);
					}
				}

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					for (int i = 0; i < count(); i++)
					{
						QWidget* w = widget(i);
						ViewBase* vb = dynamic_cast<ViewBase*>(w);
						if (vb != nullptr)
							vb->setDataModel(model);
					}
				}

				void clearAll();
				void refreshAll();
				void needsRefreshAll();

				ViewType viewType() {
					return view_;
				}

				void setViewType(ViewType index);

				QWidget* getWidget(int index);

				QWidget* getWidget(ViewType t) {
					return getWidget(static_cast<int>(t));
				}

				QWidget* currentWidget() {
					return getWidget(view_);
				}

				void deleteWidget(int index);

				bool isForm(int index) {
					return index > static_cast<int>(ViewType::CustomDataSet);
				}

				bool createFetchFormView(const QString& key, const QString& title, QColor c, int& index);

			signals:
				void itemDoubleClicked(ViewType type, const QString& key);

			private:
				void teamItemDoubleClicked(QTreeWidgetItem* item, int col);
				void matchItemDoubleClicked(QTreeWidgetItem* item, int col);

			private:
				ViewType view_;
				std::map<QString, QWidget*> scouting_forms_;
				GameFieldManager field_mgr_;
				ImageManager images_;
			};
		}
	}
}
