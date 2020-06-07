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



#include "AdvListWidget.h"
#include <QKeyEvent>
#include <QMimeData>

AdvListWidget::AdvListWidget(QWidget* parent) : QListWidget(parent)
{

}

AdvListWidget::~AdvListWidget()
{

}

void AdvListWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key::Key_Delete)
	{
		int row = currentRow();
		if (row >= 0) {
			QListWidgetItem* item = takeItem(row);
			QString name = item->text();
			delete item;
		}
	}
}

void AdvListWidget::dropEvent(QDropEvent* ev)
{
	if (ev->mimeData()->hasFormat("text")) {
		QString item = QString::fromUtf8(ev->mimeData()->data("text"));
		QStringList list = item.split(",");
		for (const QString& item : list) {
			if (findItems(item, Qt::MatchExactly).size() == 0) {
				QListWidgetItem* i = new QListWidgetItem(item, this);
				addItem(i);

				emit itemChanged(i);
			}
		}
	}
}

void AdvListWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void AdvListWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void AdvListWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}