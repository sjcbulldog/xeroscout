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
#include "ScoutingDataModel.h"
#include "ViewBase.h"
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QTextEdit>
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

                QVariant matchValue(const QString& name);
                void createMatchesDataSet();

            private:
                QComboBox* box_;
                QGroupBox* report_;
                QTextEdit* report_txt_;
                QString current_team_;
                xero::scouting::datamodel::ScoutingDataSet matches_ds_;
            };
        }
    }
}
