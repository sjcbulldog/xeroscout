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
