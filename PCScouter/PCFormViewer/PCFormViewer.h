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

#include "DocumentView.h"
#include "ImageManager.h"
#include <QMainWindow>
#include <QMainWindow>
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QFrame>
#include <QString>
#include <QTextEdit>
#include <QSettings>
#include <QIcon>
#include <QUuid>
#include <QLabel>
#include <QMenu>

class PCFormViewer : public QMainWindow
{
    Q_OBJECT

public:
    PCFormViewer(QWidget *parent = Q_NULLPTR);

private:
    void createMenus();
    void createWindows();

    QIcon loadIcon(const QString& filename);
    void listItemChanged(QListWidgetItem* olditem, QListWidgetItem* newitem);

    void loadForm();

protected:
    void closeEvent(QCloseEvent* ev);

private:
    static constexpr const char* GeometrySettings = "geometry";
    static constexpr const char* WindowStateSettings = "windows";
    static constexpr const char* LeftRightSplitterSettings = "leftright";
    static constexpr const char* TopBottomSplitterSettings = "topbottom";

private:
    void updateWindow();

private:
	QSplitter* left_right_splitter_;
	QListWidget* view_selector_;
    QSplitter* top_bottom_splitter_;
	xero::scouting::views::DocumentView* view_frame_;
    QTextEdit* logwin_;

    QMenu* file_menu_;
    QMenu* edit_menu_;

    QSettings settings_;

    xero::scouting::datamodel::ImageManager images_;

    std::shared_ptr<xero::scouting::datamodel::ScoutingForm> form_;
};
