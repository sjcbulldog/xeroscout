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
