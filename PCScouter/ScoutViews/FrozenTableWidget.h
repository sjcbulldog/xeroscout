#pragma once

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QGridLayout>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class FrozenTableWidget : public QWidget
			{
				Q_OBJECT

			public:
				FrozenTableWidget(QWidget* parent);
				virtual ~FrozenTableWidget();

				void syncFrozen();

				QTableWidget* table() {
					return main_;
				}

				bool frozenRows() const {
					return rows_frozen_ != nullptr;
				}

				bool frozenCols() const {
					return cols_frozen_ != nullptr;
				}

				void freezeRows(int rows);
				void unfreezeRows();
				void freezeCols(int cols);
				void unfreezeCols();

			private:
				void updateLayout();
				void verticalChanged(int value);

			private:
				QTableWidget* main_;
				QTableWidget* rows_frozen_;
				QTableWidget* cols_frozen_;

				QGridLayout* layout_;
			};
		}
	}
}
