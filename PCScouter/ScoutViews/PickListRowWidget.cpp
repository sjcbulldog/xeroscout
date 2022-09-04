#include "PickListRowWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListRowWidget::PickListRowWidget(const PickListEntry& entry, int rank, QWidget *parent) : QWidget(parent), entry_(entry), rank_(rank)
			{
				setMaximumSize(QSize(MaximumWidth, MaximumHeight));
				setMinimumSize(QSize(MinimumWidth, MinimumHeight));
				background_not_selected_ = QColor(242, 222, 167);
				background_selected_ = QColor(240, 240, 255);
				background_not_available_ = QColor(164, 164, 164);
				background_third_ = QColor(192, 192, 255);
				background_third_selected_ = QColor(164, 164, 164);
				setMouseTracking(true);
				setAcceptDrops(true);
				third_ = -1;
				current_ = false;
				droppos_ = -1;
			}

			PickListRowWidget::~PickListRowWidget()
			{
			}

			void PickListRowWidget::putDrag(int which)
			{
				entry_.insertThird(which, dragging_team_, dragging_score_);
				update();
			}

			void PickListRowWidget::dragEnterEvent(QDragEnterEvent* ev)
			{
				const QMimeData* data = ev->mimeData();
				if (!data->hasText())
					return;

				QString txt = data->text();
				if (txt.length() <= 5)
					return;

				if (txt.startsWith("PL:t"))
				{
					QStringList list = txt.split(',');
					if (list.size() != 3)
						return;

					int rank = list[1].toInt();
					int thired = list[2].toInt();

					if (rank != rank_)
						return;

					ev->acceptProposedAction();
				}
			}

			void PickListRowWidget::dragLeaveEvent(QDragLeaveEvent* ev)
			{
				droppos_ = -1;
			}

			void PickListRowWidget::dragMoveEvent(QDragMoveEvent* ev)
			{
				const QMimeData* data = ev->mimeData();
				if (!data->hasText())
					return;

				QString txt = data->text();
				if (txt.length() <= 5)
					return;

				if (txt.startsWith("PL:t"))
				{
					QStringList list = txt.split(',');
					if (list.size() != 3)
						return;

					int rank = list[1].toInt();
					int thired = list[2].toInt();

					if (rank != rank_)
						return;

					ev->acceptProposedAction();
					int which = xposToThird(ev->pos().x());
					if (which != droppos_)
					{
						droppos_ = which;
						repaint();
					}
				}
			}

			void PickListRowWidget::dropEvent(QDropEvent* ev)
			{
				if (droppos_ != -1)
				{
					diddrop_ = true;
					putDrag(droppos_);
					ev->acceptProposedAction();
					ev->accept();
					emit rowChanged(rank_ - 1);
				}
			}

			void PickListRowWidget::mousePressEvent(QMouseEvent* ev)
			{
				diddrop_ = false;

				if (ev->button() == Qt::LeftButton)
				{
					QDrag* drag = new QDrag(this);
					QMimeData* data = new QMimeData();
					QPixmap mp;

					assert(current_ == true);
					if (third_ == -1)
					{
						//
						// Let the parent handle the drag event
						//
						emit dragRow(ev->pos(), rank_ - 1);
						return;
					}

					QString d = "PL:t," + QString::number(rank_) + "," + QString::number(third_);
					data->setText(d);

					mp = QPixmap(ThirdAreaWidth, height());
					QPainter paint(&mp);
					paintThirdArea(paint, third_, false);

					dragging_index_ = third_;
					third_ = -1;
					dragging_team_ = entry_.thirdTeam(dragging_index_);
					dragging_score_ = entry_.thirdScore(dragging_index_);

					entry_.removeThird(dragging_index_);
					update();

					int xpos = TeamAreaWidth + LeftMargin + static_cast<int>(dragging_index_) * ThirdAreaWidth;
					int ypos = TopMargin;

					drag->setMimeData(data);
					drag->setPixmap(mp);
					QPoint pt(ev->pos().x() - xpos, ev->pos().y() - ypos);
					drag->setHotSpot(pt);

					Qt::DropAction act = drag->exec(Qt::MoveAction);
					if (!diddrop_)
						putDrag(dragging_index_);
				}
			}

			int PickListRowWidget::xposToThird(int x)
			{
				int which = -1;
				if (x > TeamAreaWidth)
				{
					which = (x - TeamAreaWidth) / ThirdAreaWidth;
					if (which >= NumberThirds)
						which = -1;
				}

				return which;
			}

			void PickListRowWidget::mouseMoveEvent(QMouseEvent* ev)
			{
				int which = xposToThird(ev->pos().x());
				if (which != third_)
				{
					third_ = which;
					update();
				}
			}

			void PickListRowWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter paint(this);

				paintBackground(paint);
				paintContent(paint);
				paintDropPos(paint);
			}

			void PickListRowWidget::paintDropPos(QPainter& paint)
			{
				if (droppos_ == -1)
					return;

				paint.save();
				int xpos = TeamAreaWidth + LeftMargin + droppos_ * ThirdAreaWidth;
				QPen pen(QColor(0, 0, 0));
				pen.setWidth(6);
				paint.setPen(pen);
				paint.drawLine(xpos, 0, xpos, height());
				paint.restore();
			}

			void PickListRowWidget::paintThirdArea(QPainter& paint, int i, bool window)
			{
				int xpos;
				QString str;
				int teamwidth;
				int scorewidth;
				QPoint pt;
				bool paintback = false;
				QColor backcolor;

				QFont bigf = paint.font();
				bigf.setPointSizeF(9.0);
				QFontMetrics bigfm(bigf);

				QFont mediumf = paint.font();
				mediumf.setPointSizeF(7.5);
				QFontMetrics mediumfm(mediumf);

				QFont smallf = paint.font();
				smallf.setPointSizeF(6.0);
				QFontMetrics smallfm(smallf);
				
				if (window)
					xpos = TeamAreaWidth + LeftMargin + static_cast<int>(i) * ThirdAreaWidth;
				else
					xpos = 0;

				int team = entry_.thirdTeam(i);
				if (selected_.count(team) > 0)
				{
					paintback = true;
					backcolor = background_third_selected_;
				}
				else if ((i == third_ && window) || !window)
				{
					paintback = true;
					backcolor = background_third_;
				}

				if (paintback)
				{
					paint.save();
					QRect r(xpos, TopMargin, ThirdAreaWidth, height() - TopMargin * 2);
					QBrush b(backcolor);
					paint.setBrush(b);
					paint.setPen(Qt::PenStyle::NoPen);
					paint.drawRect(r);
					paint.restore();
				}

				double score = entry_.thirdScore(i);

				str = QString::number(team);
				teamwidth = mediumfm.horizontalAdvance(str);
				paint.setFont(mediumf);
				pt = QPoint(xpos + ThirdAreaWidth / 2 - teamwidth / 2, TopMargin + bigfm.height());
				paint.drawText(pt, str);

				str = QString::number(score);
				scorewidth = smallfm.horizontalAdvance(str);
				paint.setFont(smallf);
				pt = QPoint(xpos + ThirdAreaWidth / 2 - scorewidth / 2, height() - BottomMargin);
				paint.drawText(pt, str);
			}

			void PickListRowWidget::paintContent(QPainter& paint)
			{
				QPoint pt;
				QString str;

				paint.save();

				QFont extrabigf = paint.font();
				extrabigf.setPointSizeF(14.0);
				QFontMetrics extrabigfm(extrabigf);

				QFont bigf = paint.font();
				bigf.setPointSizeF(9.0);
				QFontMetrics bigfm(bigf);

				QFont smallf = paint.font();
				smallf.setPointSizeF(6.0);
				QFontMetrics smallfm(smallf);

				str = QString::number(rank_);
				paint.setFont(extrabigf);
				pt = QPoint(LeftMargin, TopMargin + (height() - TopMargin - BottomMargin) / 2 + (extrabigfm.height() - extrabigfm.descent()) / 2);
				paint.drawText(pt, str);

				str = "Team " + QString::number(entry_.team());
				paint.setFont(bigf);
				pt = QPoint(LeftMargin + NumberWidth, TopMargin + bigfm.height());
				paint.drawText(pt, str);
				int teamwidth = bigfm.horizontalAdvance(str);

				str = QString::number(entry_.score());
				int scorewidth = smallfm.horizontalAdvance(str);
				paint.setFont(smallf);
				pt = QPoint(LeftMargin + NumberWidth + teamwidth / 2 - scorewidth / 2, height() - BottomMargin);
				paint.drawText(pt, str);

				size_t count = entry_.count();
				if (count > NumberThirds)
					count = NumberThirds;

				for (size_t i = 0; i < count; i++)
				{
					paintThirdArea(paint, static_cast<int>(i), true);
				}

				paint.restore();
			}

			void PickListRowWidget::paintBackground(QPainter &paint, bool forceoff)
			{
				QPen pen = QPen(QColor(0, 0, 0));
				QBrush b;
				
				if (selected_.count(entry_.team()))
					b = QBrush(background_not_available_);
				else if (current_ && !forceoff)
					b = QBrush(background_selected_);
				else
					b = QBrush(background_not_selected_);

				paint.save();
				paint.setPen(pen);
				paint.setBrush(b);

				int w = width();
				int h = height();
				paint.drawRect(0, 0, w, h);

				paint.restore();
			}

			void PickListRowWidget::enterEvent(QEnterEvent* ev)
			{
				current_ = true;
				update();
			}

			void PickListRowWidget::leaveEvent(QEvent* ev)
			{
				current_ = false;
				third_ = -1;
				update();
			}
		}
	}
}
