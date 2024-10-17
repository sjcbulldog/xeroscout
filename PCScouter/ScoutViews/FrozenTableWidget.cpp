#include "FrozenTableWidget.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollBar>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			FrozenTableWidget::FrozenTableWidget(QWidget* parent) : QWidget(parent) 
			{
				layout_ = new QGridLayout();
				setLayout(layout_);

				main_ = new QTableWidget(this);
				main_->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
				main_->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
				layout_->addWidget(main_, 0, 0);

				rows_frozen_ = nullptr;
				cols_frozen_ = nullptr;

				(void)connect(main_->verticalScrollBar(), &QScrollBar::valueChanged, this, &FrozenTableWidget::verticalChanged);
			}

			FrozenTableWidget::~FrozenTableWidget()
			{

			}

			void FrozenTableWidget::verticalChanged(int value)
			{
				if (cols_frozen_ != nullptr)
				{
					cols_frozen_->verticalScrollBar()->setValue(value);
				}
			}

			void FrozenTableWidget::syncFrozen()
			{
			}

			void FrozenTableWidget::freezeRows(int rows)
			{
				assert(rows_frozen_ == nullptr);
				rows_frozen_ = new QTableWidget();
				updateLayout();
			}

			void FrozenTableWidget::unfreezeRows()
			{
				assert(rows_frozen_ != nullptr);
				layout_->removeWidget(rows_frozen_);
				delete rows_frozen_;
				rows_frozen_ = nullptr;
				updateLayout();
			}

			void FrozenTableWidget::freezeCols(int cols)
			{
				assert(cols_frozen_ == nullptr);

				//
				// Create the table with the frozen columns
				//
				cols_frozen_ = new QTableWidget();
				cols_frozen_->setRowCount(main_->rowCount());
				cols_frozen_->setColumnCount(cols);
				cols_frozen_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
				cols_frozen_->setFocusPolicy(Qt::NoFocus);
				cols_frozen_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
				cols_frozen_->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);

				if (main_->horizontalScrollBar()->isVisible())
					cols_frozen_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

				//
				// Copy data from the main table to the frozen columns table
				//
				QStringList hdrs;

				//
				// colcnt - iterates over all columns in main table, even hiddle columns
				// colidx - iterates over columns in frozen columns, excluding hidden columns
				//
				int colcnt = 0, colidx = 0;
				while (colcnt < cols) {
					if (!main_->isColumnHidden(colidx)) {
						hdrs << main_->horizontalHeaderItem(colcnt)->text();
						for (int row = 0; row < main_->rowCount(); row++)
						{
							QTableWidgetItem* item = main_->item(row, colcnt);
							QTableWidgetItem* newitem = new QTableWidgetItem(item->text());
							cols_frozen_->setItem(row, colidx, newitem);
						}
						colidx++;
					}

					colcnt++;
				}

				cols_frozen_->setHorizontalHeaderLabels(hdrs);
				updateLayout();
			}

			void FrozenTableWidget::unfreezeCols()
			{
				assert(cols_frozen_ != nullptr);
				layout_->removeWidget(cols_frozen_);
				delete cols_frozen_;
				cols_frozen_ = nullptr;
				updateLayout();
			}
			
			void FrozenTableWidget::updateLayout()
			{
				layout_->removeWidget(main_);
				if (rows_frozen_ != nullptr)
					layout_->removeWidget(rows_frozen_);
				if (cols_frozen_ != nullptr)
					layout_->removeWidget(cols_frozen_);

				if (rows_frozen_ != nullptr && cols_frozen_ != nullptr) 
				{
					layout_->addWidget(main_, 1, 1);
					layout_->addWidget(rows_frozen_, 0, 1);
					layout_->addWidget(cols_frozen_, 1, 0);

					main_->verticalHeader()->hide();
					main_->horizontalHeader()->hide();
				}
				else if (rows_frozen_ != nullptr) 
				{
					layout_->addWidget(rows_frozen_, 0, 0);
					layout_->addWidget(main_, 1, 0);
					main_->horizontalHeader()->hide();
					main_->verticalHeader()->show();
				}
				else if (cols_frozen_ != nullptr) 
				{
					layout_->addWidget(cols_frozen_, 0, 0);
					layout_->addWidget(main_, 0, 1);
					main_->verticalHeader()->hide();
					main_->horizontalHeader()->show();
				}
				else 
				{
					layout_->addWidget(main_, 0, 0);
					main_->verticalHeader()->show();
					main_->horizontalHeader()->show();
				}

				if (cols_frozen_ != nullptr)
				{
					cols_frozen_->resizeColumnsToContents();
					int width = 0;
					for (int i = 0; i < cols_frozen_->columnCount(); i++) {
						int dwidth = cols_frozen_->columnWidth(i);
						int hwidth = cols_frozen_->horizontalHeader()->sectionSize(i);
						width += std::max(dwidth, hwidth);
					}
					width += cols_frozen_->verticalHeader()->width();
					cols_frozen_->setFixedWidth(width);
				}
			}
		}
	}
}
