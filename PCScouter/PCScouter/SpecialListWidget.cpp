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

#include "SpecialListWidget.h"
#include <QKeyEvent>

SpecialListWidget::SpecialListWidget(QWidget* parent) : QListWidget(parent)
{
}

SpecialListWidget::~SpecialListWidget()
{
}

void SpecialListWidget::keyPressEvent(QKeyEvent* ev)
{
	QListWidget::keyPressEvent(ev);

	keyword_ += ev->text();
	if (keyword_.length() > 10)
		keyword_ = keyword_.right(10);

	if (keyword_.length() >= 5 && keyword_.right(5) == "xyzzy")
	{
		emit magicWord(Word::XYZZY);
		keyword_.clear();
	}
	else if (keyword_.length() >= 6 && keyword_.right(6) == "plough")
	{
		emit magicWord(Word::PLOUGH);
		keyword_.clear();
	}
	else if (keyword_.length() >= 3 && keyword_.right(3) == "dpr")
	{
		emit magicWord(Word::DPR);
		keyword_.clear();
	}
}