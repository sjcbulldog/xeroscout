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
	setBackgroundRole(QPalette::Window);
}

void AdvListWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}