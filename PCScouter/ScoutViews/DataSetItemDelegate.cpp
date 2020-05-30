#include "DataSetItemDelegate.h"
#include <QLineEdit>
#include <QIntValidator>
#include <QComboBox>
#include <QDoubleValidator>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			DataSetItemDelegate::DataSetItemDelegate(std::shared_ptr<FieldDesc> hdr)
			{
				hdr_ = hdr;
			}

			DataSetItemDelegate::~DataSetItemDelegate()
			{
			}

			QWidget* DataSetItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
			{
				QWidget* ret = nullptr;
				QValidator* val;

				switch (hdr_->type())
				{
				case FieldDesc::Type::Boolean:
					{
						QComboBox* box = new QComboBox(parent);
						box->addItem("true");
						box->addItem("false");
						ret = box;
					}
					break;
				case FieldDesc::Type::Double:
					{
						QLineEdit* edit = new QLineEdit(parent);
						val = new QDoubleValidator(ret);
						edit->setValidator(val);
						ret = edit;
					}
					break;
				case FieldDesc::Type::Integer:
					{
						QLineEdit* edit = new QLineEdit(parent);
						val = new QIntValidator(ret);
						edit->setValidator(val);
						ret = edit;
					}
					break;
				case FieldDesc::Type::String:
					ret = new QLineEdit(parent);
					break;
					case FieldDesc::Type::StringChoice:
					{
						QComboBox* box = new QComboBox(parent);
						for (auto item : hdr_->choices())
							box->addItem(item);

						ret = box;
					}
					break;
				}

				return ret;
			}
		}
	}
}
