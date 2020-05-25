#include "ExpressionsEntryDialog.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ExpressionsEntryDialog::ExpressionsEntryDialog(const QStringList& vars, QWidget* parent) : QDialog(parent)
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				QWidget* top = new QWidget();
				QHBoxLayout* hlay = new QHBoxLayout();
				top->setLayout(hlay);
				QLabel* label = new QLabel("Expression", top);
				hlay->addWidget(label);
				edit_ = new QLineEdit(top);
				hlay->addWidget(edit_);

				vlay->addWidget(top);

				vars_ = new QListWidget(this);
				vlay->addWidget(vars_);
				connect(vars_, &QListWidget::itemDoubleClicked, this, &ExpressionsEntryDialog::doubleClicked);

				QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
				box_ = new QDialogButtonBox(buttons, this);
				vlay->addWidget(box_);

				QPushButton* b;
				b = box_->button(QDialogButtonBox::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = box_->button(QDialogButtonBox::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);

				for (const QString& var : vars)
				{
					QListWidgetItem* item = new QListWidgetItem(var, vars_);
					vars_->addItem(item);
				}
			}

			ExpressionsEntryDialog::~ExpressionsEntryDialog()
			{
			}

			void ExpressionsEntryDialog::doubleClicked(QListWidgetItem* item)
			{
				edit_->setText(edit_->text() + item->text());
				edit_->setFocus();
			}
		}
	}
}
