#pragma once

#include <QWidget>
#include <QListWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			class DataSetFieldPopup : public QWidget
			{
				Q_OBJECT

			public:
				DataSetFieldPopup(const QStringList& fields, QWidget* parent = Q_NULLPTR);
				~DataSetFieldPopup();

				const QString& selected() const {
					return selected_;
				}

			private:
				void doubleClicked(QListWidgetItem* item);

			private:
				QListWidget* list_;
				QString selected_;
			};
		}
	}
}
