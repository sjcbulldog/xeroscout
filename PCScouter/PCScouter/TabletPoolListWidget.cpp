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

#include "TabletPoolListWidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QMimeData>
#include <QMessageBox>

TabletPoolListWidget::TabletPoolListWidget(QWidget* parent) : QListWidget(parent)
{
	setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
	setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);

	connect(this, &QListWidget::itemChanged, this, &TabletPoolListWidget::itemRenamed);
}

TabletPoolListWidget::~TabletPoolListWidget()
{
}

void TabletPoolListWidget::itemRenamed(QListWidgetItem* item)
{
	//
	// See if the name is valid, if not, we kick off the edit cycle again
	//
	const QString &text = item->text();

	if (!text[0].isLetter())
	{
		QMessageBox::critical(this, "Error", "A tablet name must start with a letter and consists of letters, numbers, and '_'");
		item->setText(oldname_);
		return;
	}

	for (int i = 1; i < text.length(); i++)
	{
		if (!text[i].isLetterOrNumber() && text[i] != '_')
		{
			QMessageBox::critical(this, "Error", "A tablet name must start with a letter and consists of letters, numbers, and '_'");
			item->setText(oldname_);
			return;
		}
	}

	emit tabletRenamed(oldname_, item->text());
}

void TabletPoolListWidget::insertNewTablet()
{
	QListWidgetItem* item;

	int i = 1;
	QString name;
	while (true) {
		name = "New_Tablet_" + QString::number(i);
		if (findItems(name, Qt::MatchExactly).size() == 0)
			break;
		i++;
	}

	item = new QListWidgetItem(name, this);
	item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
	addItem(item);
}

void TabletPoolListWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key::Key_Delete)
	{
		// Signal table delete
		auto indexes = selectedIndexes();
		std::sort(indexes.begin(), indexes.end(), [](const QModelIndex& a, const QModelIndex& b)->bool {return a.row() > b.row(); });
		for (const QModelIndex& index : indexes) {
			int row = index.row();
			auto item = takeItem(row);
			QString name = item->text();
			delete item;
			emit tabletDeleted(name);
		}
	}
	else if (ev->key() == Qt::Key::Key_Insert)
	{
		insertNewTablet();
	}
}

void TabletPoolListWidget::mouseDoubleClickEvent(QMouseEvent* ev)
{
	QListWidgetItem* item = itemAt(ev->pos());
	if (item != nullptr)
	{
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		if (item != nullptr) {
			oldname_ = item->text();
			editItem(item);
		}
	}
}

QMimeData* TabletPoolListWidget::mimeData(const QList<QListWidgetItem*>& items) const
{
	QMimeData* data = new QMimeData();
	QString str;

	for (auto i : items) {
		if (str.length() > 0)
			str += ",";
		str += i->text();
	}
	data->setData("text", str.toUtf8());

	return data;
}
