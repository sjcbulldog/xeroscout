#include "BluetoothConnectDialog.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTreeWidget>

BluetoothConnectDialog::BluetoothConnectDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	QPushButton* b = ui.buttons_->button(QDialogButtonBox::Ok);
	connect(b, &QPushButton::pressed, this, &QDialog::accept);

	b = ui.buttons_->button(QDialogButtonBox::Cancel);
	connect(b, &QPushButton::pressed, this, &QDialog::reject);

	ui.devices_->setHeaderHidden(true);

	setWindowTitle("Searching for XeroScout central machines ...");

	connect(ui.devices_, &QTreeWidget::itemDoubleClicked, this, &BluetoothConnectDialog::doubleClicked);
}

BluetoothConnectDialog::~BluetoothConnectDialog()
{
}

void BluetoothConnectDialog::addDevice(const QString& name)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.devices_);
	item->setText(0, name);
	ui.devices_->addTopLevelItem(item);
}

void BluetoothConnectDialog::doubleClicked(QTreeWidgetItem* item, int col)
{
	emit selected(item->text(0));
}
