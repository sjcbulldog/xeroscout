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
#include "ScoutingDataMap.h"
#include "ScoutingForm.h"
#include <QDialog>
#include <QTableWidget>
#include <QDialogButtonBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ScoutDataMergeDialog : public QDialog
			{
			public:
				ScoutDataMergeDialog(std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form, const std::list<xero::scouting::datamodel::ConstScoutingDataMapPtr>& list);
				virtual ~ScoutDataMergeDialog();

				xero::scouting::datamodel::ScoutingDataMapPtr result() {
					return result_;
				}

				void accept() override;

			private:
				void populate();

			private:
				QTableWidget* table_;
				QDialogButtonBox* box_;
				std::list<xero::scouting::datamodel::ConstScoutingDataMapPtr> list_;
				xero::scouting::datamodel::ScoutingDataMapPtr result_;
				std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form_;
			};
		}
	}
}


