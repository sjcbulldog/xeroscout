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

#include "DataSetViewWidget.h"
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QBoxLayout>
#include <QScrollBar>
#include <QHeaderView>
#include <cmath>

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			bool DataSetViewWidgetItem::operator<(const QTableWidgetItem& other) const
			{
				bool ret;
				bool okme, okother;

				const QString& metext = text();
				const QString& othertext = other.text();

				double meval = metext.toDouble(&okme);
				double otherval = othertext.toDouble(&okother);

				if (okme && okother)
					ret = (meval < otherval);
				else
					ret = (metext < othertext);

				return ret;
			}

			DataSetViewWidget::DataSetViewWidget(QWidget* parent) : QSplitter(parent)
			{
				direction_ = true;
				column_ = -1;

				setOrientation(Qt::Horizontal);

				left_ = new QTableWidget(this);
				left_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				addWidget(left_);

				right_ = new QTableWidget(this);
				right_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				right_->verticalHeader()->hide();
				addWidget(right_);

				connect(left_->horizontalHeader(), &QHeaderView::sectionClicked, this, &DataSetViewWidget::sortLeftData);
				connect(right_->horizontalHeader(), &QHeaderView::sectionClicked, this, &DataSetViewWidget::sortRightData);

				connect(left_->verticalScrollBar(), &QAbstractSlider::valueChanged, right_->verticalScrollBar(), &QAbstractSlider::setValue);

				connect(right_->verticalScrollBar(), &QAbstractSlider::valueChanged, left_->verticalScrollBar(), &QAbstractSlider::setValue);
			}

			DataSetViewWidget::~DataSetViewWidget()
			{
			}

			void DataSetViewWidget::sortRightData(int col)
			{
				if (col == column_)
				{
					direction_ = !direction_;
				}
				else
				{
					direction_ = true;
					column_ = col;
				}

				right_->sortItems(column_, direction_ ? Qt::AscendingOrder : Qt::DescendingOrder);
				left_->sortItems(column_, direction_ ? Qt::AscendingOrder : Qt::DescendingOrder);
			}

			void DataSetViewWidget::sortLeftData(int col)
			{
				if (col == column_)
				{
					direction_ = !direction_;
				}
				else
				{
					direction_ = true;
					column_ = col;
				}

				right_->sortItems(column_, direction_ ? Qt::AscendingOrder : Qt::DescendingOrder);
				left_->sortItems(column_, direction_ ? Qt::AscendingOrder : Qt::DescendingOrder);
			}

			void DataSetViewWidget::refreshView()
			{
				updateData(left_);
				updateData(right_);
			}

			void DataSetViewWidget::updateData(QTableWidget* table)
			{
				table->setColumnCount(data_.columnCount());
				table->setRowCount(data_.rowCount());

				QStringList headers;
				for (const QString& hdr : data_.headers())
				{
					QString newhdr = hdr;
					newhdr = newhdr.replace("/", "\n");
					headers.push_back(newhdr);
				}

				table->setHorizontalHeaderLabels(headers);

				for (int row = 0; row < data_.rowCount(); row++) {
					for (int col = 0; col < data_.columnCount(); col++) {
						QString str;
						QVariant v = data_.get(row, col);
						if (v.type() == QVariant::Type::Double)
						{
							double intpart;
							double value = v.toDouble();

							if (std::modf(value, &intpart) < 0.0001)
							{
								str = QString::number(static_cast<int>(value));
							}
							else if (std::fabs(value) < 1.0)
							{
								str = QString::number(v.toDouble(), 'f', 4);
							}
							else
							{
								str = QString::number(v.toDouble(), 'f', 1);
							}
						}
						else
						{
							str = data_.get(row, col).toString();
						}
						DataSetViewWidgetItem* item = new DataSetViewWidgetItem(str);
						item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
						table->setItem(row, col, item);
					}
				}

				table->resizeColumnsToContents();
				table->setUpdatesEnabled(true);
			}
		}
	}
}
