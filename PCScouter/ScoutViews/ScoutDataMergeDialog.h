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


