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

#include "GraphConfigurationDialog.h"
#include <QBoxLayout>
#include <QPushButton>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			GraphConfigurationDialog::GraphConfigurationDialog(const GraphDescriptor& desc) : desc_(desc)
			{
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				which_ = new QComboBox(this);
				lay->addWidget(which_);

				holder_ = new QWidget(this);
				lay->addWidget(holder_);

				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				holder_->setSizePolicy(p);

				QHBoxLayout* hlay = new QHBoxLayout();
				holder_->setLayout(hlay);

				src_ = new QListWidget(holder_);
				hlay->addWidget(src_);

				graph_ = new QListWidget(holder_);
				hlay->addWidget(graph_);

				QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
				buttons_ = new QDialogButtonBox(buttons, this);
				lay->addWidget(buttons_);

				QPushButton* b;
				b = buttons_->button(QDialogButtonBox::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = buttons_->button(QDialogButtonBox::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);
			}

			GraphConfigurationDialog::~GraphConfigurationDialog()
			{
			}
		}
	}
}
