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
