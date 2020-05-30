#pragma once
#include "FieldDesc.h"
#include <QItemDelegate>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class DataSetItemDelegate : public QItemDelegate
			{
			public:
				DataSetItemDelegate(std::shared_ptr<xero::scouting::datamodel::FieldDesc> hdr);
				virtual ~DataSetItemDelegate();

				QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

			private:
				std::shared_ptr<xero::scouting::datamodel::FieldDesc> hdr_;
			};
		}
	}
}
