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

				Q_OBJECT

			public:
				DataSetViewWidget(const QString &name, bool editable, QWidget* parent);
				virtual ~DataSetViewWidget();

				xero::scouting::datamodel::ScoutingDataSet& dataset() override {
					return data_;
				}

				virtual void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					ViewBase::setDataModel(model);

					if (model != nullptr)
						model->datasetColumnOrder(name_, colstate_, colgeom_);
				}

				void refreshView();

				void clearView() {
					data_.clear();
					refreshView();
				}

				bool hasDataSet() override {
					return true;
				}

			signals:
				void rowChanged(int row, int col);

			private:
				void updateData(QTableWidget* w);
				void sortData(int column);
				void contextMenuRequested(const QPoint& pt);
				void hideColumn();
				void unhideColumns();
				void resetColumns();

				void columnMoved(int logindex, int oldindex, int newindex);
				void updateColumnOrder();

				void itemChanged(QTableWidgetItem* item);
				void setDelegates();

			private:
				QString name_;
				QPoint pt_;

				xero::scouting::datamodel::ScoutingDataSet data_;
				QTableWidget* table_;

				int column_;
				bool direction_;

				QByteArray colstate_;
				QByteArray colgeom_;

				bool editable_;
			};
		}
	}
}
