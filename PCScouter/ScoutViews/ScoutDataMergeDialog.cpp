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

#include "ScoutDataMergeDialog.h"
#include "ScoutingDataMap.h"
#include "ChoiceFormItem.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QComboBox>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			ScoutDataMergeDialog::ScoutDataMergeDialog(std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form, const std::list<xero::scouting::datamodel::ConstScoutingDataMapPtr> &list)
			{
				form_ = form;

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				table_ = new QTableWidget(this);
				lay->addWidget(table_);

				box_ = new QDialogButtonBox(this);
				box_->addButton(QDialogButtonBox::Cancel);
				box_->addButton(QDialogButtonBox::Ok);
				lay->addWidget(box_);

				list_ = list;

				(void)connect(box_->button(QDialogButtonBox::StandardButton::Cancel), &QPushButton::pressed, this, &QDialog::reject);
				(void)connect(box_->button(QDialogButtonBox::StandardButton::Ok), &QPushButton::pressed, this, &ScoutDataMergeDialog::accept);

				populate();
			}

			ScoutDataMergeDialog::~ScoutDataMergeDialog()
			{
			}

			void ScoutDataMergeDialog::accept()
			{
#ifdef MERGE_DATA_NOT_READY
				//
				// Extract the data from the last column of the
				//
				QStringList headers;
				for (auto pair : *list_.front())
					headers.push_back(pair.first);


				result_ = std::make_shared<ScoutingDataMap>();
				int col = table_->columnCount() - 1;
				for (int row = 0; row < list_.front()->size(); row++)
				{
					std::shared_ptr<FormItemDesc> fitem = form_->itemByName(headers.at(row));
					auto it = list_.back()->find(headers.at(row));
					const QString& name = headers.at(row);

					if (fitem == nullptr)
					{
						//
						// Copy the data from the back
						//
						result_->insert_or_assign(name, it->second.toString());
					}
					else
					{
						if (fitem->dataType() == QVariant::Type::Int)
						{
							QString txt = table_->item(row, col)->text();
							int num = txt.toInt();
							result_->insert_or_assign(name, QVariant(num));
						}
						else if (fitem->dataType() == QVariant::Type::Bool)
						{
							bool value = false;

							QComboBox* box = dynamic_cast<QComboBox*>(table_->cellWidget(row, col));
							if (box != nullptr)
							{
								if (box->currentIndex() == 0)
									value = true;
							}

							result_->insert_or_assign(name, QVariant(value));
						}
						else if (fitem->dataType() == QVariant::Type::String)
						{
							QString value = "";

							QComboBox* box = dynamic_cast<QComboBox*>(table_->cellWidget(row, col));
							if (box != nullptr)
								value = box->currentText();

							result_->insert_or_assign(name, QVariant(value));
						}
						else
						{
							assert(false);
						}
					}
				}
#endif
				QDialog::accept();
			}

			void ScoutDataMergeDialog::populate()
			{
				table_->setColumnCount(list_.size() + 2);
				table_->setRowCount(list_.front()->size());

				QStringList headers;
				for (auto pair : *list_.front())
					headers.push_back(pair.first);

				//
				// The first column, which are the names of the item
				//
				for (int i = 0; i < headers.size(); i++)
				{
					QTableWidgetItem* item = new QTableWidgetItem(headers[i]);
					item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
					table_->setItem(i, 0, item);
				}

				//
				// Now the middle columns which is the data from the various scoutdatamap elements
				//
				int col = 1;
				for (auto data : list_)
				{
					for (int row = 0; row < data->size(); row++)
					{
						auto it = data->find(headers.at(row));
						assert(it != data->end());

						QTableWidgetItem* item = new QTableWidgetItem(it->second.toString());
						item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
						table_->setItem(row, col, item);
					}

					col++;
				}

				//
				// Now the final column, which is editable version for the results
				// of the merge operation
				//
				for (int row = 0; row < list_.front()->size(); row++)
				{
					std::shared_ptr<FormItemDesc> fitem = form_->itemByName(headers.at(row));
					auto it = list_.back()->find(headers.at(row));

					if (fitem == nullptr)
					{
						//
						// Added data, not part of the original form, not editable
						//
						QTableWidgetItem* item = new QTableWidgetItem(it->second.toString());
						item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
						table_->setItem(row, col, item);
					}
					else
					{
#ifdef MERGE_DATA_NOT_READY
						if (fitem->dataType() == QVariant::Type::Int)
						{
							QTableWidgetItem* item = new QTableWidgetItem(it->second.toString());
							item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable);
							table_->setItem(row, col, item);
						}
						else if (fitem->dataType() == QVariant::Type::Bool)
						{
							QComboBox* box = new QComboBox();
							box->addItem("TRUE");
							box->addItem("FALSE");
							table_->setCellWidget(row, col, box);

							if (it->second.toBool() == true)
								box->setCurrentIndex(0);
							else
								box->setCurrentIndex(1);
						}
						else if (fitem->dataType() == QVariant::Type::String)
						{
							int index = -1;
							std::shared_ptr<ChoiceFormItem> choice = std::dynamic_pointer_cast<ChoiceFormItem>(fitem);
							if (choice == nullptr)
							{
								//
								// Plain text item
								//
								QTableWidgetItem* item = new QTableWidgetItem(it->second.toString());
								item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable);
								table_->setItem(row, col, item);
							}
							else
							{
								QComboBox* box = new QComboBox();
								for (const QString& c : choice->choices())
								{
									box->addItem(c);
								}
								box->setCurrentText(it->second.toString());
								table_->setCellWidget(row, col, box);
							}
						}
						else
						{
							assert(false);
						}
#endif
					}
				}

				
				//
				// Now, resize columns
				//
				table_->resizeColumnsToContents();

				//
				// Highlight differences in blue to make them stand out
				//
				for (int row = 0; row < table_->rowCount(); row++)
				{
					bool same = true;
					QString first;

					for (col = 1; col < table_->columnCount() - 1; col++)
					{
						if (col == 1)
						{
							first = table_->item(row, col)->text();
						}
						else
						{
							if (table_->item(row, col)->text() != first)
							{
								same = false;
								break;
							}
						}
					}

					if (!same)
					{
						for (col = 1; col < table_->columnCount() - 1; col++)
						{
							auto item = table_->item(row, col);
							item->setBackgroundColor(QColor(154, 221, 252, 255));
						}
					}
				}
			}
		}
	}
}
