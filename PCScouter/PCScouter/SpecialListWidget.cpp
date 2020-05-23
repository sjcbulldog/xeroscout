//
// Copyright 2020 by Jack W. (Butch) Griffin
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
		emit magicWord(Word::XYZZY);
	else if (keyword_.length() >= 6 && keyword_.right(6) == "plough")
		emit magicWord(Word::PLOUGH);
}