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
#include "DataSetViewWidget.h"
#include "ScoutingDataModel.h"
#include "ScoutingDatabase.h"
#include "ViewBase.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include <QCompleter>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT QueryViewWidget : public QWidget, public ViewBase
			{
			public:
				QueryViewWidget(QWidget* parent);
				virtual ~QueryViewWidget();

				void clearView();
				void refreshView();

				void madeActive();

				bool hasDataSet() override {
					return true;
				}

				xero::scouting::datamodel::ScoutingDataSet& dataset() {
					return data_view_->dataset();
				}

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					ViewBase::setDataModel(model);
					if (data_view_ != nullptr)
						data_view_->setDataModel(model);
				}

			private:
				void executeQuery();

			private:
				QLineEdit* query_widget_;
				QPushButton* query_execute_;
				DataSetViewWidget* data_view_;
				QCompleter* completer_;
			};

		}
	}
}
