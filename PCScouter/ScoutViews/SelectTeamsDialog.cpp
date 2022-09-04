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
				tree_->sortByColumn(1, Qt::AscendingOrder);
				tree_->blockSignals(false);
			}
		}
	}
}
