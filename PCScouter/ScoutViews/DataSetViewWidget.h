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
#include "ScoutingDataSet.h"
#include "ViewBase.h"
#include <QSplitter>
#include <QTableWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class DataSetViewWidgetItem : public QTableWidgetItem
			{
			public:
				DataSetViewWidgetItem(const QString& str) : QTableWidgetItem(str)
				{
				}

				bool operator<(const QTableWidgetItem& other) const override;
			};

			class SCOUTVIEWS_EXPORT DataSetViewWidget : public QSplitter, public ViewBase
			{
				friend class DataSetViewWidgetItem;

			public:
				DataSetViewWidget(QWidget* parent);
				virtual ~DataSetViewWidget();

				xero::scouting::datamodel::ScoutingDataSet& dataset() {
					return data_;
				}

				void refreshView();

				void clearView() {
					data_.clear();
					refreshView();
				}

			private:
				void updateData(QTableWidget* w);
				void sortLeftData(int column);
				void sortRightData(int column);
				void contextMenuRequestedLeft(const QPoint& pt);
				void contextMenuRequestedRight(const QPoint& pt);
				void contextMenuRequested(const QPoint& pt, bool leftside);
				void hideColumn();
				void unhideColumns();

			private:
				QPoint pt_;
				bool left_side_;

				xero::scouting::datamodel::ScoutingDataSet data_;
				QTableWidget* right_;
				QTableWidget* left_;

				int column_;
				bool direction_;

			};

		}
	}
}
