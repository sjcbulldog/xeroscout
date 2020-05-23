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

#include "QueryViewWidget.h"
#include "DelimitedCompleter.h"
#include <QMessageBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			QueryViewWidget::QueryViewWidget(QWidget* parent) : QWidget(parent)
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				QWidget* top = new QWidget(this);
				QHBoxLayout* hlay = new QHBoxLayout();
				top->setLayout(hlay);

				query_widget_ = new QLineEdit(top);
				hlay->addWidget(query_widget_);

				query_execute_ = new QPushButton("Execute", top);
				hlay->addWidget(query_execute_);
				(void)connect(query_execute_, &QPushButton::pressed, this, &QueryViewWidget::executeQuery);
				(void)connect(query_widget_, &QLineEdit::returnPressed, this, &QueryViewWidget::executeQuery);

				vlay->addWidget(top);

				data_view_ = new DataSetViewWidget(this);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				data_view_->setSizePolicy(p);
				vlay->addWidget(data_view_);
				completer_ = nullptr;
			}

			QueryViewWidget::~QueryViewWidget()
			{
			}

			void QueryViewWidget::clearView()
			{
				data_view_->clearView();
			}

			void QueryViewWidget::madeActive()
			{
				if (completer_ != nullptr) 
				{
					query_widget_->setCompleter(nullptr);
					delete completer_;
				}

				QStringList list = dataModel()->getAllFieldNames();
				list.append("SELECT");
				list.append("WHERE");
				list.append("FROM");
				list.append("AND");
				list.append("OR");
				list.append("matches");
				list.append("teams");

				completer_ = new DelimitedCompleter(query_widget_, ' ', list);
				query_widget_->setCompleter(completer_);
			}

			void QueryViewWidget::refreshView()
			{
				auto data_model = dataModel();

				if (data_model == nullptr)
				{
					data_view_->clearView();
				}
				else
				{
					QString err;
					if (!data_model->createCustomDataSet(data_view_->dataset(), query_widget_->text(), err))
						return;
				}
				data_view_->refreshView();;
			}

			void QueryViewWidget::executeQuery()
			{
				auto data_model = dataModel();

				if (data_model == nullptr)
				{
					data_view_->clearView();
					return;
				}

				QString err;
				if (!data_model->createCustomDataSet(data_view_->dataset(), query_widget_->text(), err))
				{
					QMessageBox::critical(this, "SQL Error", err);
					return;
				}

				data_view_->refreshView();
			}
		}
	}
}
