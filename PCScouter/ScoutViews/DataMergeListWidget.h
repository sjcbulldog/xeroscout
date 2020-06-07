//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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
#include <QWidget>
#include <QTreeWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT DataMergeListWidget : public QWidget, public ViewBase
			{
				Q_OBJECT 

			public:
				DataMergeListWidget(QWidget* parent = 0);
				virtual ~DataMergeListWidget();

				virtual void clearView() ;
				virtual void refreshView() ;

			signals:
				void mergeMatch(const QString& key, xero::scouting::datamodel::Alliance c, int slot);
				void mergePit(const QString& key);

			private:
				void doubleClicked(QTreeWidgetItem* item, int col);

			private:
				QTreeWidget* tree_;
			};
		}
	}
}
