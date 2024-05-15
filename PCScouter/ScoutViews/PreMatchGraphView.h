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

#include "scoutviews_global.h"
#include "GraphPerTeamView.h"
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT PreMatchGraphView : public GraphPerTeamView
			{
			public:
				PreMatchGraphView(QWidget* parent);
				virtual ~PreMatchGraphView();

				virtual void refreshView() {
					if (!changeTriggered())
					{
						updateComboBox();
						refreshCharts();
					}
				}

				virtual void clearView() {
				}

				static constexpr const char* Name = "match";

			protected:
				void createTop(QGroupBox* top);

			private:
				void matchChanged();
				void updateComboBox();

			private:
				QComboBox* box_;
				QString current_match_;
				QStringList team_keys_;
			};
		}
	}
}

