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

#include "UpDownWidget.h"
#include <QEvent>
#include <QIntValidator>
#include <QFont>
#include <QFontMetrics>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			UpDownWidget::UpDownWidget(const QString& label, QWidget* parent) : QWidget(parent)
			{
				label_ = new QLabel(label, this);

				up_ = new QPushButton("-", this);
				(void)connect(up_, &QPushButton::pressed, this, &UpDownWidget::upButtonPressed);

				down_ = new QPushButton("+", this);
				(void)connect(down_, &QPushButton::pressed, this, &UpDownWidget::downButtonPressed);

				editor_ = new QLineEdit(this);

				QFont font = editor_->font();
				font.setPointSizeF(20.0);
				editor_->setFont(font);

				font.setPointSizeF(16.0);
				setFont(font);

				QFontMetrics fm(font);
				int width = qMax(150, fm.horizontalAdvance(label_->text()));
				setMinimumSize(QSize(width, 90));

				doLayout();

				minv_ = std::numeric_limits<int>::min();
				maxv_ = std::numeric_limits<int>::max();

				setValue(0);

				editor_->setValidator(new QIntValidator(minv_, maxv_, this));
			}

			UpDownWidget::~UpDownWidget()
			{
			}

			int UpDownWidget::value()
			{
				return editor_->text().toInt();
			}

			void UpDownWidget::setValue(int v)
			{
				if (v < minv_)
					editor_->setText(QString::number(minv_));
				else if (v > maxv_)
					editor_->setText(QString::number(maxv_));
				else
					editor_->setText(QString::number(v));
			}

			void UpDownWidget::upButtonPressed()
			{
				int v = value() - 1;
				if (v >= minv_ && v <= maxv_)
					editor_->setText(QString::number(v));
			}

			void UpDownWidget::downButtonPressed()
			{
				int v = value() + 1;
				if (v >= minv_ && v <= maxv_)
					editor_->setText(QString::number(v));
			}

			void UpDownWidget::changeEvent(QEvent* ev)
			{
				if (ev->type() == QEvent::FontChange)
					doLayout();
			}

			void UpDownWidget::resizeEvent(QResizeEvent* ev)
			{
				doLayout();
			}

			void UpDownWidget::doLayout()
			{
				int button_width = 30;
				int button_height = 30;

				QFontMetrics fm(font());

				QRect r = QRect(0, 0, width(), fm.height());
				label_->setGeometry(r);

				int remaining = height() - fm.height();
				r = QRect(width() - button_width, fm.height(), button_width, remaining / 2);
				down_->setGeometry(r);

				r = QRect(width() - button_width, fm.height() + remaining / 2, button_width, remaining / 2);
				up_->setGeometry(r);

				r = QRect(0, fm.height(), width() - button_width, height() - fm.height());
				editor_->setGeometry(r);
			}
		}
	}
}
