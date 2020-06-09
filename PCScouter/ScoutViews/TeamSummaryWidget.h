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
#include "ScoutingDataModel.h"
#include "ViewBase.h"
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QTextEdit>
#include <QTextBrowser>
#include <QCheckBox>
#include <memory>

namespace xero
{
    namespace scouting
    {
        namespace views
        {
            class SCOUTVIEWS_EXPORT TeamSummaryWidget : public QWidget, public ViewBase
            {
                Q_OBJECT

            public:
                TeamSummaryWidget(QWidget* parent);
                virtual ~TeamSummaryWidget();

                void refreshView();
                void clearView();

            protected:
                void showEvent(QShowEvent* ev) override;

            private:
                void teamChanged(int index);
                void regenerate(bool force = false);
                void contextMenu(const QPoint& pos);

                void addVariable(const QString& var);
                void removeVariable(const QString& var);

                QString generateTitle();
                QString generateMatchRecord();
                QString generateTeamSummary();

                void createMatchesDataSet();

                void videoLinkClicked(const QUrl& link);

            private:
                QComboBox* box_;
                QGroupBox* report_;
                QTextBrowser* report_txt_;
                QString current_team_;
                xero::scouting::datamodel::ScoutingDataSet matches_ds_;
            };
        }
    }
}
