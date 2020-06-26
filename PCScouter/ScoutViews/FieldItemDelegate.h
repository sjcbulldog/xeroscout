#pragma once

#include "ScoutingDataSet.h"
#include <QItemDelegate>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class FieldItemDelegate : public QItemDelegate
			{
			public:
				FieldItemDelegate(xero::scouting::datamodel::ScoutingDataSet& set);
				virtual ~FieldItemDelegate();

				QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override ;
				void setEditorData(QWidget* editor, const QModelIndex& index) const override;
				void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override ;

			private:
				xero::scouting::datamodel::ScoutingDataSet& data_;
			};
		}
	}
}
