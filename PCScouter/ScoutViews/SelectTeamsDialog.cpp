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

#include "SelectTeamsDialog.h"
#include <QBoxLayout>
#include <QPushButton>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			SelectTeamsDialog::SelectTeamsDialog(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, const QStringList& keys)
			{
				QStringList headers;
				headers << "";
				headers << "Rank";
				headers << "Number";
				headers << "Name";
			
				dm_ = dm;
				keys_ = keys;

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				tree_ = new QTreeWidget(this);
				tree_->setHeaderLabels(headers);
				tree_->setColumnCount(4);
				connect(tree_, &QTreeWidget::itemChanged, this, &SelectTeamsDialog::itemChanged);
				lay->addWidget(tree_);

				QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
				box_ = new QDialogButtonBox(buttons, this);
				lay->addWidget(box_);

				QPushButton* b;

				b = box_->button(QDialogButtonBox::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = box_->button(QDialogButtonBox::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);

				populateTeams();
			}

			SelectTeamsDialog::~SelectTeamsDialog()
			{
			}

			void SelectTeamsDialog::itemChanged(QTreeWidgetItem *item, int col)
			{
				QString key = item->data(0, Qt::UserRole).toString();

				if (item->checkState(0) == Qt::CheckState::Checked)
				{
					if (!keys_.contains(key))
						keys_.push_back(key);
				}
				else
				{
					if (keys_.contains(key))
						keys_.removeOne(key);
				}
			}

			void SelectTeamsDialog::populateTeams()
			{
				tree_->blockSignals(true);
				tree_->clear();

				for (auto t : dm_->teams())
				{
					int index = 0;
					QTreeWidgetItem* item = new SelectTeamsWidgetItem(tree_);

					item->setText(index++, "");

					if (t->hasRank())
						item->setText(index++, QString::number(t->rank()));
					else
						item->setText(index++, "-");

					item->setText(index++, QString::number(t->number()));
					item->setText(index++, t->name());

					if (keys_.contains(t->key()))
						item->setCheckState(0, Qt::CheckState::Checked);
					else
						item->setCheckState(0, Qt::CheckState::Unchecked);

					item->setData(0, Qt::UserRole, t->key());
				}

				tree_->resizeColumnToContents(0);
				tree_->resizeColumnToContents(1);
				tree_->resizeColumnToContents(2);
				tree_->sortByColumn(1);
				tree_->blockSignals(false);
			}
		}
	}
}
