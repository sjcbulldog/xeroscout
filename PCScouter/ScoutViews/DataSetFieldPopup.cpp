#include "DataSetFieldPopup.h"
#include <QBoxLayout>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			DataSetFieldPopup::DataSetFieldPopup(const QStringList &fields, QWidget* parent) : QWidget(parent)
			{
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				list_ = new QListWidget(this);
				lay->addWidget(list_);

				setMaximumWidth(600);
				setMinimumWidth(200);
				setMaximumHeight(800);
				setMinimumHeight(400);

				for (const QString& f : fields)
				{
					QListWidgetItem* item = new QListWidgetItem(f);
					list_->addItem(item);
				}

				connect(list_, &QListWidget::itemDoubleClicked, this, &DataSetFieldPopup::doubleClicked);
			}

			DataSetFieldPopup::~DataSetFieldPopup()
			{
			}

			void DataSetFieldPopup::doubleClicked(QListWidgetItem* item)
			{
				selected_ = list_->currentItem()->text();
				close();
			}
		}
	}
}
