//
// Copyright 2020 by Jack W. (Butch) Griffin
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
#include "ViewBase.h"
#include <QTreeWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ChangeHistoryView : public QTreeWidget, public ViewBase
			{
			public:
				ChangeHistoryView(QWidget* parent);
				virtual ~ChangeHistoryView();

				void refreshView();
				void clearView() {
					clear();
				}

			protected:
				void showEvent(QShowEvent* ev) override;

			private:
				static QStringList headers_;
			};

		}
	}
}
