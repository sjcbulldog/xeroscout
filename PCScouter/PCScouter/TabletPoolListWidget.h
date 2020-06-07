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
#include <QListWidget>
#include <QLineEdit>

class TabletPoolListWidget : public QListWidget
{
	Q_OBJECT;

public:
	TabletPoolListWidget(QWidget* parent);
	virtual ~TabletPoolListWidget();

signals:
	void tabletDeleted(const QString& name);
	void tabletRenamed(const QString& oldname, const QString& newname);

protected:
	void mouseDoubleClickEvent(QMouseEvent* ev) override;
	void keyPressEvent(QKeyEvent* ev) override;
	QMimeData* mimeData(const QList<QListWidgetItem*> items) const;


private:
	void insertNewTablet();
	void itemRenamed(QListWidgetItem* item);

private:
	QString oldname_;
};

