#pragma once

#include <QDialog>
#include "ui_BluetoothConnectDialog.h"

class BluetoothConnectDialog : public QDialog
{
	Q_OBJECT

public:
	BluetoothConnectDialog(QWidget *parent = Q_NULLPTR);
	~BluetoothConnectDialog();

	void addDevice(const QString& name);

signals:
	void selected(const QString& name);

private:
	void doubleClicked(QTreeWidgetItem* item, int col);

private:
	Ui::BluetoothConnectDialog ui;
};
