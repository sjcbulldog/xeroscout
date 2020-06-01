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

QMimeData* TabletPoolListWidget::mimeData(const QList<QListWidgetItem*> items) const
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
