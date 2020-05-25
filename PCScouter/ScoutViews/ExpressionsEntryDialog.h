#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QListWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ExpressionsEntryDialog : public QDialog
			{
			public:
				ExpressionsEntryDialog(const QStringList& vars, QWidget* parent);
				virtual ~ExpressionsEntryDialog();

				QString expr() const {
					return edit_->text();
				}

			private:
				void doubleClicked(QListWidgetItem* item);

			private:
				QLineEdit* edit_;
				QListWidget* vars_;
				QDialogButtonBox* box_;
			};
		}
	}
}

