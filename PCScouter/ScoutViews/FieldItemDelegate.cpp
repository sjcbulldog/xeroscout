#include "FieldItemDelegate.h"
#include <QComboBox>
#include <QSize>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			FieldItemDelegate::FieldItemDelegate(ScoutingDataSet& set) : data_(set)
			{
			}

			FieldItemDelegate::~FieldItemDelegate()
			{
			}

			QWidget* FieldItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
			{
				QComboBox* box = new QComboBox(parent);
				box->addItems(data_.fieldNames());
				box->setEditable(true);

				return box;
			}

			void FieldItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
			{
				QItemDelegate::setEditorData(editor, index);
			}

			void FieldItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
			{
				QItemDelegate::updateEditorGeometry(editor, option, index);

				QSize s = editor->sizeHint();

				QRect r = editor->geometry();
				r.setHeight(s.height());
				editor->setGeometry(r);
			}
		}
	}
}
