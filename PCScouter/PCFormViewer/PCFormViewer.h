#pragma once

#include "DocumentView.h"
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
	QSplitter* left_right_splitter_;
	QListWidget* view_selector_;
    QSplitter* top_bottom_splitter_;
	xero::scouting::views::DocumentView* view_frame_;
    QTextEdit* logwin_;

    QMenu* file_menu_;

    QSettings settings_;
};
