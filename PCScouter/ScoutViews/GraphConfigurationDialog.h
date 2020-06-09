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

#pragma once

#include "GraphDescriptor.h"
#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QDialogButtonBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class GraphConfigurationDialog : public QDialog
			{
			public:
				GraphConfigurationDialog(const xero::scouting::datamodel::GraphDescriptor& desc);
				virtual ~GraphConfigurationDialog();

			private:
				QComboBox* which_;
				QWidget* holder_;
				QListWidget* src_;
				QListWidget* graph_;
				QDialogButtonBox* buttons_;

				xero::scouting::datamodel::GraphDescriptor desc_;
			};
		}
	}
}
