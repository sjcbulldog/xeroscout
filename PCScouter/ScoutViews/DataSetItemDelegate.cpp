//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

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
