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

#include "EditName.h"
#include <QKeyEvent>

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			EditName::EditName(QWidget* parent) : QLineEdit(parent)
			{
			}

			EditName::~EditName()
			{
			}

			void EditName::keyPressEvent(QKeyEvent* ev)
			{
				if (ev->key() == Qt::Key_Escape)
				{
					emitEscapePressed();
					return;
				}

				QLineEdit::keyPressEvent(ev);
			}

			void EditName::emitEscapePressed()
			{
				emit escapePressed();
			}
		}
	}
}
