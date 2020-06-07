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

#include "ViewBase.h"
#include <QWidget>
#include <QTextBrowser>
#include <QTabWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PickListView : public QWidget, public ViewBase
			{
			public:
				PickListView(const QString& name, QWidget* parent);
				virtual ~PickListView();

				void setHTML(const QString& picks, const QString &robots) {
					picks_ = picks;
					picks_win_->setHtml(picks_);

					robots_ = robots;
					robot_win_->setHtml(robots_);
				}

				virtual void clearView() ;
				virtual void refreshView() ;

			private:
				QTabWidget* tabs_;
				QTextBrowser* picks_win_;
				QTextBrowser* robot_win_;
				QString picks_;
				QString robots_;
			};
		}
	}
}
