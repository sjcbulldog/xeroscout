#include "TeamSelectedWidget.h"
#include <QPainter>
#include <QMouseEvent>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			TeamSelectedWidget::TeamSelectedWidget(QWidget* parent) : QWidget(parent)
			{
				back_color_unselected_ = QColor(204, 255, 229);
				back_color_selected_ = QColor(211, 211, 211);
				updateSize();
			}

			TeamSelectedWidget::~TeamSelectedWidget()
			{
			}

			void TeamSelectedWidget::updateSize()
			{
				int count = static_cast<int>(teams_.size());
				int width = TeamBoxWidth + LeftMargin + RightMargin;
				int height = count * TeamBoxHeight + count * TeamBoxSpacing + TopMargin + BottomMargin;
				setMinimumSize(QSize(width, height));
				setMaximumSize(QSize(width, height));
			}

			void TeamSelectedWidget::mousePressEvent(QMouseEvent* ev)
			{
				int which = (ev->pos().y() - TopMargin) / (TeamBoxHeight + TeamBoxSpacing);
				int offset = (ev->pos().y() - TopMargin) % (TeamBoxHeight + TeamBoxSpacing);

				if (which < teams_.size() && offset < TeamBoxHeight)
				{
					auto it = teams_.begin();
					std::advance(it, which);
					it->second = !it->second;
					update();

					teamStateChanged(it->first, it->second);
				}
			}

			void TeamSelectedWidget::paintEvent(QPaintEvent*)
			{
				QPainter paint(this);
				QPen pen(QColor(0, 0, 0));
				int y = TopMargin;
				QRect r;
				QFont font = paint.font();
				QFontMetrics fm(font);
				QBrush unsel(back_color_unselected_);
				QBrush sel(back_color_selected_);

				pen.setWidth(2);

				paint.setPen(pen);


				for (auto pair : teams_)
				{
					r = QRect(LeftMargin, y, TeamBoxWidth, TeamBoxHeight);
					if (pair.second)
						paint.setBrush(sel);
					else
						paint.setBrush(unsel);

					paint.drawRect(r);

					QString str = QString::number(pair.first);
					int ypos = y + TeamBoxHeight / 2 + (fm.height() - fm.descent()) / 2;
					QPoint pt(LeftMargin + TeamBoxWidth / 2 - fm.horizontalAdvance(str) / 2, ypos);
					paint.drawText(pt, str);

					y += TeamBoxHeight + TeamBoxSpacing;
				}
			}
		}
	}
}