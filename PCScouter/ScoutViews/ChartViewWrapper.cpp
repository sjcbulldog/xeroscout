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

#include "ChartViewWrapper.h"
#include <QBarSet>
#include <QDebug>
#include <QToolTip>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ChartViewWrapper::ChartViewWrapper(QWidget* parent, std::shared_ptr<GraphDescriptor::GraphPane> pane) : QChartView(parent)
			{
				editor_ = nullptr;
				pane_ = pane;
			}

			ChartViewWrapper::~ChartViewWrapper()
			{
			}

			void ChartViewWrapper::mouseMoveEvent(QMouseEvent* ev)
			{
				QChartView::mouseMoveEvent(ev);
				pt_ = ev->globalPos();
			}

			void ChartViewWrapper::mouseHovered(bool status, int index, QBarSet* set)
			{
				if (status)
				{
					QString txt = QString::number(set->at(index));
					QToolTip::showText(pt_, txt);
				}
				else
				{
					QToolTip::hideText();
				}
			}

			void ChartViewWrapper::mouseDoubleClickEvent(QMouseEvent* ev)
			{
				QFontMetrics fm(chart()->titleFont());
				int size = fm.horizontalAdvance(chart()->title());

				if (ev->pos().y() < chart()->margins().top() + fm.height())
				{
					int c = chart()->rect().center().x();
					if (ev->pos().x() > c - size / 2 && ev->pos().x() < c + size / 2)
					{
						editTitle();
					}
				}
			}

			void ChartViewWrapper::editTitle()
			{
				if (editor_ == nullptr)
				{
					editor_ = new EditName(this);
					(void)connect(editor_, &EditName::returnPressed, this, &ChartViewWrapper::editorDone);
					(void)connect(editor_, &EditName::escapePressed, this, &ChartViewWrapper::editorAborted);
					editor_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}

				QRect r(20, 28, width() - 40, 40);
				editor_->setGeometry(r);
				editor_->setText(chart()->title());
				editor_->setVisible(true);
				editor_->setFocus();
				editor_->selectAll();
			}

			void ChartViewWrapper::editorDone()
			{
				chart()->setTitle(editor_->text());
				editor_->setVisible(false);

				emit titleChanged(pane_, editor_->text());
			}

			void ChartViewWrapper::editorAborted()
			{
				editor_->setVisible(false);
			}
		}
	}
}
