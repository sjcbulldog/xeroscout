#include "ImageView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QCursor>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			std::vector<QColor> ImageView::colors_ =
			{
				QColor(255, 0, 0),
				QColor(0, 255, 0),
				QColor(0, 0, 255),
				QColor(252, 186, 3),
				QColor(144,252,3),
				QColor(240, 252, 3),
				QColor(3,252,252),
				QColor(219, 3, 252),
				QColor(252,3,132)
			};

			ImageView::ImageView(const QString& name, std::shared_ptr<QImage> image, QWidget* parent) : QWidget(parent), ViewBase(name)
			{
				number_ = -1;
				multiplier_ = 1.0;
				image_ = image;
				current_color_ = 0;
				draw_size_ = 16;
				drawing_ = false;

				setColorCursor();
			}

			ImageView::~ImageView()
			{
			}

			void ImageView::setColorCursor()
			{
				QPixmap map(draw_size_, draw_size_);
				map.fill(Qt::transparent);
				QPainter paint(&map);
				QBrush brush(colors_[current_color_]);
				paint.setBrush(brush);
				paint.drawEllipse(0, 0, draw_size_, draw_size_);
				QCursor cursor(map);
				setCursor(cursor);
			}

			void ImageView::mousePressEvent(QMouseEvent* ev)
			{
				if (ev->button() == Qt::MouseButton::RightButton)
				{
					current_color_ = (current_color_ + 1) % colors_.size();
					setColorCursor();
				}
				else if (ev->button() == Qt::MouseButton::LeftButton)
				{
					drawing_ = true;
					current_line_.color_ = colors_[current_color_];
					current_line_.draw_size_ = draw_size_;
					current_line_.points_.push_back(ev->pos());
				}
			}

			void ImageView::mouseMoveEvent(QMouseEvent* ev)
			{
				if (drawing_)
				{
					current_line_.points_.push_back(ev->pos());
					update();
				}
			}

			void ImageView::mouseReleaseEvent(QMouseEvent* ev)
			{
				drawing_ = false;
				if (current_line_.points_.size() > 0)
				{
					lines_.push_back(current_line_);
					current_line_.points_.clear();
				}
			}

			void ImageView::mouseDoubleClickEvent(QMouseEvent* ev)
			{
				lines_.clear();
				current_line_.points_.clear();
				update();
			}

			void ImageView::refreshView()
			{
			}

			void ImageView::clearView()
			{
			}

			void ImageView::computeMultiplier()
			{
				double m;

				if (image_ == nullptr)
				{
					m = 1.0;
				}
				else
				{
					QSize s = image_->size();
					double mx = (double)width() / (double)s.width();
					double my = (double)height() / (double)s.height();

					if (mx < my)
						m = my;
					else
						m = mx;
				}

				if (m != multiplier_)
				{
					multiplier_ = m;
					update();
				}
			}

			void ImageView::paintEvent(QPaintEvent* ev)
			{
				QPainter paint(this);

				QSize s = image_->size();
				s = s * multiplier_;
				QRect r(QPoint(0, 0), s);

				qDebug() << r;

				paint.drawImage(r, *image_); 

				if (current_line_.points_.size() > 0)
				{
					QPen pen(current_line_.color_);
					pen.setWidth(current_line_.draw_size_);
					paint.setPen(pen);
					for (size_t i = 1; i < current_line_.points_.size(); i++)
					{
						paint.drawLine(current_line_.points_[i - 1], current_line_.points_[i]);
					}
				}

				for (const line& l : lines_)
				{
					QPen pen(l.color_);
					pen.setWidth(l.draw_size_);
					paint.setPen(pen);
					for (size_t i = 1; i < l.points_.size(); i++)
					{
						paint.drawLine(l.points_[i - 1], l.points_[i]);
					}
				}
			}

			void ImageView::resizeEvent(QResizeEvent* ev)
			{
				computeMultiplier();
			}
		}
	}
}
