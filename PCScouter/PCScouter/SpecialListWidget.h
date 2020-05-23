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

#pragma once
#include <QListWidget>

class SpecialListWidget : public QListWidget
{
	Q_OBJECT

public:
	enum class Word
	{
		XYZZY,
		PLOUGH,
	};

public:
	SpecialListWidget(QWidget* parent);
	virtual ~SpecialListWidget();

signals:
	void magicWord(Word w);

protected:
	void keyPressEvent(QKeyEvent* ev) override;

private:
	QString keyword_;
};

