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
    void updatePerForm(std::shared_ptr<xero::scouting::datamodel::ScoutingForm> form);

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

    std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model_;
    std::shared_ptr<xero::scouting::datamodel::ScoutingForm> form_;
};
