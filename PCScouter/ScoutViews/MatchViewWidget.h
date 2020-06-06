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
#include "DataModelMatch.h"
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

			class SCOUTVIEWS_EXPORT MatchViewWidget : public QTreeWidget, public ViewBase
			{
			public:
				MatchViewWidget(const QString &tablet, QWidget* parent);
				virtual ~MatchViewWidget();

				void setData(std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelMatch>> matches);
				void clearView();
				void refreshView();
				void setScoutingField(const QString& key, xero::scouting::datamodel::Alliance c, int slot);

				void setTablet(const QString& tablet) override {
					tablet_ = tablet;
					refreshView();
				}

			private:
				static bool wayToSort(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> one, std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> two);
				static int matchClass(const QString& comp);
				void putOneAll(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match);
				void putOneOne(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match);
				int putAllianceData(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, QTreeWidgetItem* i, xero::scouting::datamodel::Alliance a, int index);

				QString genStatusText(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match, xero::scouting::datamodel::Alliance a, int slot);

			private:
				QString tablet_;
				std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelMatch>> matches_;

				static QStringList headers_all_;
				static QStringList headers_one_;
			};

		}
	}
}
