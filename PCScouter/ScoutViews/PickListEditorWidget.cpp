#include "PickListEditorWidget.h"
#include <QPainter>
#include <QScrollArea>
#include <QMimeData>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDebug>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListEditorWidget::PickListEditorWidget(QWidget* parent) : QWidget(parent)
			{
				left_margin_ = 8;
				right_margin_ = 8;
				top_margin_ = 12;
				bottom_margin_ = 36;
				between_gap_ = 12;

				setSizePolicy(QSizePolicy::QSizePolicy::Policy::Maximum, QSizePolicy::QSizePolicy::Policy::Maximum);
				setAcceptDrops(true);
				setMouseTracking(true);
			}

			PickListEditorWidget::~PickListEditorWidget()
			{
			}

			void PickListEditorWidget::dragEnterEvent(QDragEnterEvent* ev)
			{
				const QMimeData* data = ev->mimeData();
				if (!data->hasText())
					return;

				QString txt = data->text();
				if (txt.length() <= 5)
					return;

				if (txt.startsWith("PL:r"))
				{
					ev->acceptProposedAction();
				}
			}

			void PickListEditorWidget::dragLeaveEvent(QDragLeaveEvent* ev)
			{
			}

			void PickListEditorWidget::dragMoveEvent(QDragMoveEvent* ev)
			{
				const QMimeData* data = ev->mimeData();
				if (!data->hasText())
					return;

				QString txt = data->text();
				if (txt.length() <= 5)
					return;

				if (txt.startsWith("PL:r") && rows_.size() != 0)
				{
					QWidget* wid = rows_[0];

					int which = (ev->pos().y() - top_margin_ + wid->height() / 2) / (wid->height() + between_gap_);
					if (which < 0 || which >= rows_.size())
						which = -1;

					ev->acceptProposedAction();

					if (which != droppos_)
					{
						droppos_ = which;
						update();
					}
				}
			}

			void PickListEditorWidget::dropEvent(QDropEvent* ev)
			{
				const QMimeData* data = ev->mimeData();
				if (!data->hasText())
					return;

				QString txt = data->text();
				if (txt.length() <= 5)
					return;

				if (txt.startsWith("PL:r") && rows_.size() != 0 && droppos_ != -1)
				{
					QStringList list = txt.split(',');
					if (list.size() != 2)
						return;

					int drag = list[1].toInt();
					if (drag != droppos_)
					{
						if (drag > droppos_)
						{
							PickListEntry entry = picklist_[drag];
							picklist_.erase(picklist_.begin() + drag);
							picklist_.insert(picklist_.begin() + droppos_, entry);
						}
						else if (drag >= picklist_.size())
						{
							PickListEntry entry = picklist_[drag];
							picklist_.erase(picklist_.begin() + drag);
							picklist_.push_back(entry);
						}
						else
						{
							PickListEntry entry = picklist_[drag];
							picklist_.erase(picklist_.begin() + drag);
							picklist_.insert(picklist_.begin() + droppos_ - 1, entry);
						}

						emit picklistChanged();
						droppos_ = -1;
						recreateChildren();
					}
				}
			}

			void PickListEditorWidget::paintEvent(QPaintEvent *ev)
			{
				QPainter paint(this);

				QBrush br(QColor(226, 255, 226));
				paint.setBrush(br);
				paint.drawRect(0, 0, width(), height());

				if (rows_.size() > 0 && droppos_ != -1)
				{
					QWidget* w = rows_[0];
					int ypos = top_margin_ + (w->height() + between_gap_) * droppos_ + -between_gap_ / 2;
					QPen pen(QColor(0, 0, 0));
					paint.setPen(pen);
					pen.setWidth(4);

					paint.drawLine(left_margin_, ypos, width() - right_margin_, ypos);
				}
			}

			void PickListEditorWidget::recreateChildren()
			{
				for (auto row : rows_)
					delete row;

				rows_.clear();

				int rank = 1;
				for (const PickListEntry& entry : picklist_)
				{
					PickListRowWidget* widget = new PickListRowWidget(entry, rank++, this);
					rows_.push_back(widget);
					(void)connect(widget, &PickListRowWidget::rowChanged, this, &PickListEditorWidget::rowChanged);
				}
				resizeEvent(nullptr);
				doLayout(width());
			}

			void PickListEditorWidget::rowChanged()
			{
				emit picklistChanged();
			}

			void PickListEditorWidget::doLayout(int winwid)
			{
				int xpos = left_margin_;
				int ypos = top_margin_;

				for (size_t i = 0; i < rows_.size(); i++)
				{
					QWidget* widget = rows_[i];

					widget->setSizePolicy(QSizePolicy::QSizePolicy::Policy::Maximum, QSizePolicy::QSizePolicy::Policy::Maximum);

					QRect r(xpos, ypos, width(), height());
					widget->setGeometry(r);
					widget->setEnabled(true);
					widget->setVisible(true);

					ypos += widget->height() + between_gap_;
				}
			}

			void PickListEditorWidget::resizeEvent(QResizeEvent* ev)
			{
				if (picklist_.size() == 0)
				{
					QSize s(0, 0);
					setMinimumSize(s);
					setMaximumSize(s);
				}
				else
				{
					QWidget* w = rows_[0];
					int sheight = top_margin_ + static_cast<int>(rows_.size()) * w->height() + bottom_margin_;
					int swidth = left_margin_ + w->width() + right_margin_;

					QSize s(swidth, sheight);
					setMinimumSize(s);
					setMaximumSize(s);
				}
			}
		}
	}
}
