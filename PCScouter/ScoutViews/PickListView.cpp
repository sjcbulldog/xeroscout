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

#include "PickListView.h"
#include <QBoxLayout>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListView::PickListView(const QString& name, QWidget* parent) : QWidget(parent), ViewBase(name)
			{
				tabs_ = new QTabWidget();
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);
				lay->addWidget(tabs_);

				picks_win_ = new QTextBrowser(tabs_);
				tabs_->addTab(picks_win_, "PickList");

				robot_win_ = new QTextBrowser(tabs_);
				tabs_->addTab(robot_win_, "Robot Capabilities");
			}

			PickListView::~PickListView()
			{
			}

			void PickListView::clearView()
			{
				picks_win_->setHtml("");
				robot_win_->setHtml("");
			}

			void PickListView::refreshView()
			{
				picks_win_->setHtml(picks_);
				robot_win_->setHtml(robots_);
			}
		}
	}
}
