#include "ChartViewWrapper.h"
#include <QBarSet>
#include <QDebug>
#include <QToolTip>

using namespace QtCharts;
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
