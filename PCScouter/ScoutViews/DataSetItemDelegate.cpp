//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
