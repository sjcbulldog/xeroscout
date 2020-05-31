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
#include "DataSetItemDelegate.h"
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QBoxLayout>
#include <QScrollBar>
#include <QHeaderView>
#include <cmath>

using namespace xero::scouting::datamodel;

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

			DataSetViewWidget::DataSetViewWidget(const QString &name, bool editable, QWidget* parent) : QSplitter(parent), ViewBase("DataSetViewWidgetItem")
			{
				editable_ = true;
				name_ = name;
				direction_ = true;
				column_ = -1;

				setOrientation(Qt::Horizontal);

				table_ = new QTableWidget(this);
				table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				addWidget(table_);

				connect(table_, &QTableWidget::itemChanged, this, &DataSetViewWidget::itemChanged);

				connect(table_->horizontalHeader(), &QHeaderView::sectionClicked, this, &DataSetViewWidget::sortData);
				table_->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
				connect(table_->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &DataSetViewWidget::contextMenuRequested);

				table_->horizontalHeader()->setSectionsMovable(true);
				connect(table_->horizontalHeader(), &QHeaderView::sectionMoved, this, &DataSetViewWidget::columnMoved);
			}

			DataSetViewWidget::~DataSetViewWidget()
			{
			}

			void DataSetViewWidget::itemChanged(QTableWidgetItem* item)
			{
				bool changed = false;

				auto hdr = data_.colHeader(item->column());
				QString txt = item->text();

				switch (hdr->type())
				{
				case FieldDesc::Type::Boolean:
					if (txt.toLower() == "true" && !data_.get(item->row(), item->column()).toBool())
					{
						data_.set(item->row(), item->column(), QVariant(true));
						changed = true;
					}
					else if (txt.toLower() == "false" && data_.get(item->row(), item->column()).toBool())
					{
						data_.set(item->row(), item->column(), QVariant(false));
						changed = true;
					}
					break;

				case FieldDesc::Type::Integer:
					{
						int val = txt.toInt();
						if (val != data_.get(item->row(), item->column()).toInt())
						{
							data_.set(item->row(), item->column(), QVariant(val));
							changed = true;
						}
					}
					break;

				case FieldDesc::Type::Double:
					{
						double val = txt.toDouble();
						if (val != data_.get(item->row(), item->column()).toDouble())
						{
							data_.set(item->row(), item->column(), QVariant(val));
							changed = true;
						}
					}
					break;
				case FieldDesc::Type::String:
				case FieldDesc::Type::StringChoice:
					if (txt != data_.get(item->row(), item->column()))
					{
						data_.set(item->row(), item->column(), txt);
						changed = true;
					}
					break;
				}

				if (changed)
					emit rowChanged(item->row(), item->column());
			}

			void DataSetViewWidget::columnMoved(int logindex, int oldindex, int newindex)
			{
				updateColumnOrder();
			}

			void DataSetViewWidget::updateColumnOrder()
			{
				colstate_ = table_->horizontalHeader()->saveState();
				colgeom_ = table_->horizontalHeader()->saveGeometry();
				dataModel()->setDatasetColumnOrder(name_, colstate_, colgeom_);
			}

			void DataSetViewWidget::hideColumn()
			{
				QItemSelectionModel* select = table_->selectionModel();
				QModelIndexList columns = select->selectedColumns();

				if (columns.size() > 0)
				{
					for (int i = 0; i < columns.size(); i++)
					{
						auto index = columns.at(i);
						table_->setColumnHidden(index.column(), true);
					}
				}
				updateColumnOrder();
			}

			void DataSetViewWidget::unhideColumns()
			{
				for (int i = 0; i < table_->columnCount(); i++) {
					table_->setColumnHidden(i, false);
				}
				updateColumnOrder();
			}

			void DataSetViewWidget::resetColumns()
			{
				colstate_.clear();
				colgeom_.clear();
				dataModel()->setDatasetColumnOrder(name_, colstate_, colgeom_);
				refreshView();
			}

			void DataSetViewWidget::contextMenuRequested(const QPoint& pt)
			{
				QMenu* menu = new QMenu();
				QAction* act;

				pt_ = pt;
				act = menu->addAction(tr("Hide Column(s)"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::hideColumn);

				act = menu->addAction(tr("Unhide All Columns"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::unhideColumns);

				act = menu->addAction(tr("Reset Column Order"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::resetColumns);

				QPoint p = table_->mapToGlobal(pt);
				menu->exec(p);
			}

			void DataSetViewWidget::sortData(int col)
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

				table_->sortItems(column_, direction_ ? Qt::AscendingOrder : Qt::DescendingOrder);
			}

			void DataSetViewWidget::refreshView()
			{
				updateData(table_);

				if (colstate_.size() > 0)
					table_->horizontalHeader()->restoreState(colstate_);

				if (colgeom_.size() > 0)
					table_->horizontalHeader()->restoreGeometry(colgeom_);
			}

			void DataSetViewWidget::updateData(QTableWidget* table)
			{
				table->clear();

				while (table->columnCount() > 0)
					table->removeColumn(0);

				while (table->rowCount() > 0)
					table->removeRow(0);

				table->setColumnCount(data_.columnCount());
				table->setRowCount(data_.rowCount());

				QStringList headers;
				for (auto hdr : data_.headers())
				{
					headers.push_back(hdr->name());
				}
				table->setHorizontalHeaderLabels(headers);

				table->blockSignals(true);
				for (int row = 0; row < data_.rowCount(); row++) {
					for (int col = 0; col < data_.columnCount(); col++) {
						auto colhdr = data_.colHeader(col);
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
						if (colhdr->isEditable() && editable_)
							item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable);
						else
							item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);

						table->setItem(row, col, item);
					}
				}

				table->resizeColumnsToContents();
				table->setUpdatesEnabled(true);

				setDelegates();

				table->blockSignals(false);
			}

			void DataSetViewWidget::setDelegates()
			{
				for (int col = 0; col < table_->columnCount(); col++)
				{
					auto hdr = data_.colHeader(col);
					DataSetItemDelegate* del = new DataSetItemDelegate(hdr);
					table_->setItemDelegateForColumn(col, del);
				}
			}
		}
	}
}
