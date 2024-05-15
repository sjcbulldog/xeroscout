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

#include "QueryViewWidget.h"
#include <QMessageBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			QueryViewWidget::QueryViewWidget(QWidget* parent) : QWidget(parent), ViewBase("QueryViewWidget")
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

				QStringList vhdrs;
				data_view_ = new DataSetViewWidget("query", false, vhdrs, this);
				data_view_->setDataModel(dataModel());
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
