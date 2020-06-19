#include "PatternListWidget.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PatternListWidget::PatternListWidget(std::shared_ptr<QImage> enter, std::shared_ptr<QImage> exit, std::shared_ptr<QImage> idle, QWidget* parent) : QWidget(parent)
			{
				top_bottom_border_ = 4;
				text_graphics_border_ = 2;
				between_item_border_ = 2;
				image_size_ = 48;
				left_right_border_ = 4;

				idle_image_ = idle;
				enter_image_ = enter;
				exit_image_ = exit;
				selected_ = 4;
			}

			PatternListWidget::~PatternListWidget()
			{
			}

			void PatternListWidget::setRegions(std::vector<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>>& regions)
			{
				int which = 0;
				QString red = toString(Alliance::Red) + "-";
				QString blue = toString(Alliance::Blue) + "-";
				regions_ = regions;

				region_list_.clear();

				QFontMetricsF fm(font());
				int top = 0;
				int maxwidth = 0;
				for (auto r : regions_)
				{
					QString name = r->name();
					if (name.startsWith(red))
						name = name.mid(red.length());
					else if (name.startsWith(blue))
						name = name.mid(blue.length());

					int itemwidth = image_size_;
					itemwidth += text_graphics_border_;
					itemwidth += fm.horizontalAdvance(name);
					itemwidth += text_graphics_border_;
					itemwidth += image_size_;
					itemwidth += left_right_border_;

					if (itemwidth > maxwidth)
						maxwidth = itemwidth;

					top += image_size_ + between_item_border_;

					if (!region_list_.contains(name))
						region_list_.push_back(name);
				}
				top *= 2 ;
				top += 2 * top_bottom_border_;

				size_ = QSize(maxwidth, top);

				setMinimumSize(size_);
				setMaximumSize(size_);
			}

			QRect PatternListWidget::bounds(int item)
			{
				int x = 0;
				int y = top_bottom_border_ + item * (image_size_ + between_item_border_);
				return QRect(x, y, size_.width(), image_size_);
			}

			void PatternListWidget::paintOne(QPainter& painter, const QPoint &loc, std::shared_ptr<QImage> left, const QString& text, std::shared_ptr<QImage> right, bool selected)
			{
				QRect r;
				QFontMetricsF fm(painter.font());

				QRect b(loc.x(), loc.y(), size_.width(), image_size_);

				if (selected)
				{
					painter.save();
					painter.setPen(QPen(QColor(0, 0, 0)));
					painter.setBrush(QBrush(QColor(220, 220, 220)));
					painter.drawRect(b);
					painter.restore() ;
				}

				if (left != nullptr)
				{
					r = QRect(left_right_border_, b.top(), image_size_, image_size_);
					painter.drawImage(r, *left.get());
				}

				int itemwidth = fm.horizontalAdvance(text);

				QPoint p(width() / 2 - itemwidth / 2, b.top() + image_size_ / 2 + fm.descent());
				painter.drawText(p, text);

				if (right != nullptr)
				{
					r = QRect(width() - left_right_border_ - image_size_, b.top(), image_size_, image_size_);
					painter.drawImage(r, *right.get());
				}
			}

			void PatternListWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter painter(this);
				QRect r;

				int which = 0;
				QPoint pt(0, top_bottom_border_);
				for (auto rg : region_list_)
				{
					paintOne(painter, pt, enter_image_, rg, exit_image_, which == selected_);
					pt = QPoint(pt.x(), pt.y() + image_size_ + between_item_border_);
					which++;
				}

				for (auto rg : region_list_)
				{
					paintOne(painter, pt, enter_image_, rg, idle_image_, which == selected_);
					pt = QPoint(pt.x(), pt.y() + image_size_ + between_item_border_);
					which++;
				}
			}

			void PatternListWidget::mousePressEvent(QMouseEvent* ev)
			{
				mouse_down_ = true;
				mouse_down_pos_ = ev->pos();

				int item = itemAt(ev->pos());
				if (item != -1)
					selected_ = item;

				update();
			}

			QString PatternListWidget::itemToText(int item)
			{
				QString ret;

				if (item < region_list_.size())
					ret = "EnterExit:";
				else
				{
					item -= region_list_.size();
					ret = "EnterIdle:";
				}

				ret += region_list_.at(item);
				return ret;
			}

			void PatternListWidget::mouseMoveEvent(QMouseEvent* ev)
			{
				if (mouse_down_ && selected_ != -1)
				{
					QPoint d = ev->pos() - mouse_down_pos_;
					if (d.manhattanLength() > QApplication::startDragDistance())
						startDrag(selected_);
				}
			}

			void PatternListWidget::startDrag(int item)
			{
				QDrag* drag = new QDrag(this);
				QMimeData* data = new QMimeData();

				data->setText(itemToText(selected_));
				drag->setMimeData(data);

				QPixmap image(size_.width(), image_size_);
				QPainter paint(&image);

				if (selected_ >= region_list_.size())
					paintOne(paint, QPoint(0, 0), enter_image_, region_list_.at(selected_ % region_list_.size()), idle_image_, false);
				else
					paintOne(paint, QPoint(0, 0), enter_image_, region_list_.at(selected_ % region_list_.size()), exit_image_, false);
				drag->setPixmap(image);

				drag->exec();
				delete drag;
			}

			void PatternListWidget::mouseReleaseEvent(QMouseEvent* ev)
			{
				mouse_down_ = false;
			}

			int PatternListWidget::itemAt(const QPoint& pos)
			{
				if (pos.x() < left_right_border_ || pos.x() >= width() - left_right_border_)
					return -1;

				int index = (pos.y() - top_bottom_border_) / (image_size_ + between_item_border_);
				if (index >= region_list_.size() * 2)
					return -1;

				return index;
			}
		}
	}
}