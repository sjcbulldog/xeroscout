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

#include "ExpressionsEntryDialog.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ExpressionsEntryDialog::ExpressionsEntryDialog(const QStringList& vars, QWidget* parent) : QDialog(parent)
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				QWidget* top = new QWidget();
				QHBoxLayout* hlay = new QHBoxLayout();
				top->setLayout(hlay);
				QLabel* label = new QLabel("Expression", top);
				hlay->addWidget(label);
				edit_ = new QLineEdit(top);
				hlay->addWidget(edit_);

				vlay->addWidget(top);

				vars_ = new QListWidget(this);
				vlay->addWidget(vars_);
				connect(vars_, &QListWidget::itemDoubleClicked, this, &ExpressionsEntryDialog::doubleClicked);

				QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
				box_ = new QDialogButtonBox(buttons, this);
				vlay->addWidget(box_);

				QPushButton* b;
				b = box_->button(QDialogButtonBox::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = box_->button(QDialogButtonBox::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);

				for (const QString& var : vars)
				{
					QListWidgetItem* item = new QListWidgetItem(var, vars_);
					vars_->addItem(item);
				}
			}

			ExpressionsEntryDialog::~ExpressionsEntryDialog()
			{
			}

			void ExpressionsEntryDialog::doubleClicked(QListWidgetItem* item)
			{
				edit_->setText(edit_->text() + item->text());
				edit_->setFocus();
			}
		}
	}
}
