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

#include "DataSetViewWidget.h"
#include "DataSetExprContext.h"
#include "DataSetItemDelegate.h"
#include "DocumentView.h"
#include "Expr.h"
#include "DataSetRulesEditor.h"
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QBoxLayout>
#include <QScrollBar>
#include <QHeaderView>
#include <QDesktopServices>
#include <cmath>

using namespace xero::scouting::datamodel;
using namespace xero::expr;

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

			DataSetViewWidget::DataSetViewWidget(const QString &name, bool editable, QWidget* parent) : QSplitter(parent), ViewBase("DataSetViewWidgetItem"), data_(name)
			{
				editable_ = true;
				name_ = name;
				direction_ = true;
				column_ = -1;
				fn_ = nullptr;

				setOrientation(Qt::Horizontal);

				table_ = new QTableWidget(this);
				table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				table_->installEventFilter(this);
				addWidget(table_);

				connect(table_, &QTableWidget::itemChanged, this, &DataSetViewWidget::itemChanged);

				connect(table_->horizontalHeader(), &QHeaderView::sectionClicked, this, &DataSetViewWidget::sortData);
				table_->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
				connect(table_->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &DataSetViewWidget::contextMenuRequested);

				table_->horizontalHeader()->setSectionsMovable(true);
				connect(table_->horizontalHeader(), &QHeaderView::sectionMoved, this, &DataSetViewWidget::columnMoved);

				table_->setContextMenuPolicy(Qt::CustomContextMenu);
				connect(table_, &QTableWidget::customContextMenuRequested, this, &DataSetViewWidget::cellContextMenuRequested);
			}

			DataSetViewWidget::DataSetViewWidget(const QString& name, bool editable, 
				std::function<void(xero::scouting::datamodel::ScoutingDataSet& ds)> fn, QWidget* parent) : DataSetViewWidget(name, editable, parent)
			{
				fn_ = fn;
			}

			DataSetViewWidget::~DataSetViewWidget()
			{
			}

			bool DataSetViewWidget::eventFilter(QObject* o, QEvent* e)
			{
				bool ret = false;

				if (e->type() == QEvent::KeyPress)
				{
					QKeyEvent* k = dynamic_cast<QKeyEvent*>(e);
					if (k != nullptr)
					{
						if (k->key() == Qt::Key::Key_Plus && (k->modifiers() & Qt::ControlModifier) != 0)
						{
							QFont f = table_->font();
							f.setPointSizeF(f.pointSizeF() + 1.0);
							table_->setFont(f);
							table_->resizeColumnsToContents();
							table_->update();
							ret = true;
						}
						else if (k->key() == Qt::Key::Key_Minus && (k->modifiers() & Qt::ControlModifier) != 0)
						{
							QFont f = table_->font();
							f.setPointSizeF(f.pointSizeF() - 1.0);
							table_->setFont(f);
							table_->resizeColumnsToContents();
							table_->update();
							ret = true;
						}
					}
				}

				return ret;
			}

			void DataSetViewWidget::cellContextMenuRequested(const QPoint& pt)
			{
				QTableWidgetItem* item = table_->itemAt(pt);
				if (item != nullptr && item->column() >= 0 && item->column() < data_.columnCount())
				{
					auto hdr = data_.colHeader(item->column());
					if (hdr->name() == "MatchKey")
					{
						auto m = dataModel()->findMatchByKey(item->text());
						if (m != nullptr)
						{
							QMenu* menu = new QMenu();
							QAction* act;

							if (m->hasZebra())
							{
								act = menu->addAction("View Zebra Match Replay");
								auto cb = std::bind(&DataSetViewWidget::viewMatchReplay, this, m->key());
								connect(act, &QAction::triggered, cb);
							}

							if (m->youtubeKey().length() > 0)
							{
								act = menu->addAction("View Match YouTube Video");
								auto cb = std::bind(&DataSetViewWidget::viewMatchVideo, this, m->key());
								connect(act, &QAction::triggered, cb);
							}

							QPoint gpt = mapToGlobal(pt);
							menu->exec(gpt);
						}
					}
				}
			}

			void DataSetViewWidget::viewMatchReplay(const QString& key)
			{
				QObject* p = parent();

				while (p != nullptr)
				{
					DocumentView* dv = dynamic_cast<DocumentView*>(p);
					if (dv != nullptr)
					{
						dv->emitSwitchViewSignal(DocumentView::ViewType::ZebraReplayView, key);
						break;
					}

					p = p->parent();
				}
			}

			void DataSetViewWidget::viewMatchVideo(const QString& key)
			{
				auto m = dataModel()->findMatchByKey(key);
				if (m->youtubeKey().length() > 0)
				{
					QString url = "https://www.youtube.com/watch?v=" + m->youtubeKey();
					QDesktopServices::openUrl(url);
				}
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

				applyRules();
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

				QItemSelectionModel* select = table_->selectionModel();
				QModelIndexList columns = select->selectedColumns();

				pt_ = pt;
				act = menu->addAction(tr("Hide Column(s)"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::hideColumn);

				if (columns.size() == 0)
					act->setEnabled(false);

				act = menu->addAction(tr("Unhide All Columns"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::unhideColumns);

				act = menu->addAction(tr("Reset Column Order"));
				connect(act, &QAction::triggered, this, &DataSetViewWidget::resetColumns);

				if (!data_.name().startsWith("$"))
				{
					act = menu->addAction(tr("Edit Highlight Rules"));
					connect(act, &QAction::triggered, this, &DataSetViewWidget::editHighlightRules);
				}

				QPoint p = table_->mapToGlobal(pt);
				menu->exec(p);
			}

			void DataSetViewWidget::editHighlightRules()
			{
				DataSetRulesEditor editor(data_.rules(), data_);

				if (editor.exec() == QDialog::Accepted)
				{
					dataModel()->setRules(data_.name(), editor.rules());

					data_.clearRules();
					for (auto r : editor.rules())
						data_.addRule(r);

					applyRules();
				}
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
				table_->clear();
				if (dataModel() == nullptr)
					return;

				if (fn_ != nullptr)
					fn_(data_);

				updateData(table_);
				applyRules();

				if (colstate_.size() > 0)
					table_->horizontalHeader()->restoreState(colstate_);

				if (colgeom_.size() > 0)
					table_->horizontalHeader()->restoreGeometry(colgeom_);
			}

			void DataSetViewWidget::applyRules()
			{
				table_->blockSignals(true);

				for (int row = 0; row < table_->rowCount(); row++)
				{
					for (int col = 0; col < table_->columnCount(); col++)
					{
						auto i = table_->item(row, col);
						i->setToolTip("");

						QBrush br = QBrush(QColor(0, 0, 0));
						i->setForeground(br);

						br = QBrush(QColor(255, 255, 255));
						i->setBackground(br);
					}
				}

				for (auto rule : data_.rules())
					applyRule(rule);

				table_->blockSignals(false);
			}

			void DataSetViewWidget::applyRule(std::shared_ptr<const DataSetHighlightRules> rule)
			{
				std::vector<std::vector<QVariant>> combinations;
				std::map<int, std::vector<int>> rowgroups;

				data_.getCombinations(rule->fields(), combinations, rowgroups);

				//
				// Now we know the logical combinations and the rows that go with each
				//
				for (int i = 0; i < combinations.size(); i++)
				{
					//
					// Now apply the rule, per combination
					//
					const std::vector<int>& rows = rowgroups[i];

					//
					// Create a context that is per row group to evaluate the expression
					//
					DataSetExprContext context(data_, rows);
					Expr expr;

					QString err;
					if (!expr.parse(context, rule->equation(), err))
						continue;

					QVariant v;
					
					QString exprerr;
					try
					{
						v = expr.eval(context);
					}
					catch (const ExprEvalException& ex)
					{
						v = QVariant();
						exprerr = ex.what();
					}

					for (int which = 0; which < rows.size(); which++)
					{
						for (const QString& h : rule->highlights())
						{
							int col = data_.getColumnByName(h);
							if (col != -1)
							{
								if (!v.isValid() || !v.canConvert(QVariant::Bool))
								{
									QTableWidgetItem* item = table_->item(rows[which], col);
									QBrush br = QBrush(QColor(255, 255, 255));
									item->setForeground(br);

									br = QBrush(QColor(0, 0, 0));
									item->setBackground(br);

									if (exprerr.length() == 0)
										exprerr = "rule expression did not evaluate to a boolean";

									QString tt = item->toolTip();
									if (tt.length() > 0)
										tt += "\n";
									tt += exprerr;
									item->setToolTip(tt);
								}
								else if (v.toBool())
								{
									QTableWidgetItem* item = table_->item(rows[which], col);
									QBrush br = QBrush(rule->foreground());
									item->setForeground(br);

									br = QBrush(rule->background());
									item->setBackground(br);

									QString txt = item->toolTip();
									if (txt.length() > 0)
										txt += "\n";

									txt += rule->descriptor();

									item->setToolTip(txt);
								}
							}
						}
					}
				}
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
						if (colhdr->name() == DataModelTeam::TeamKeyName || colhdr->name() == DataModelMatch::MatchTeamKeyName)
						{
							auto t = dataModel()->findTeamByKey(str);
							QString txt = QString::number(t->number()) + " - " + t->nick();
							item->setToolTip(txt);
						}

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

				computeHeaderTooltips(table);

				table->blockSignals(false);
			}

			void DataSetViewWidget::computeHeaderTooltips(QTableWidget *table)
			{
				for (int col = 0; col < table->columnCount(); col++)
				{
					QString tt;

					QVariant v = data_.columnSummary(col);
					if (v.type() == QVariant::String)
					{
						tt = v.toString().replace(',', '\n');
					}
					else if (v.type() == QVariant::Double)
					{
						tt = "Average " + QString::number(v.toDouble(), 'f', 2);
					}

					table->horizontalHeaderItem(col)->setToolTip(tt);
				}
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
