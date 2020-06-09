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

#include "PathFieldView.h"
#include "Pose2d.h"
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>
#include <QCoreApplication>
#include <QFontMetricsF>
#include <QToolTip>
#include <cmath>

using namespace xero::paths;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PathFieldView::PathFieldView(QWidget* parent) : QWidget(parent)
			{
				units_ = "in";
				setMouseTracking(true);
				setFocusPolicy(Qt::ClickFocus);
				image_scale_ = 1.0;
			}

			PathFieldView::~PathFieldView()
			{
			}

			bool PathFieldView::event(QEvent* ev)
			{
				if (ev->type() == QEvent::ToolTip)
				{
					QHelpEvent* help = static_cast<QHelpEvent*>(ev);
					for (auto t : tracks_)
					{
						Translation2d pt = t->beginning();
						QPointF cursor = windowToWorld(help->pos());
						double dx = cursor.x() - pt.getX();
						double dy = cursor.y() - pt.getY();
						double dist = std::sqrt(dx * dx + dy * dy);
						if (dist < 6.0)
						{
							QToolTip::showText(help->globalPos(), t->match());
							return true;
						}
					}

					QToolTip::hideText();
					ev->ignore();
				}
				return QWidget::event(ev);
			}

			void PathFieldView::setUnits(const std::string& units)
			{
				units_ = units;
				createTransforms();
				repaint();
			}

			QSize PathFieldView::minimumSizeHint() const
			{
				return QSize(field_image_.size().width() / 4, field_image_.size().height() / 4);
			}

			QSize PathFieldView::sizeHint() const
			{
				return field_image_.size();
			}

			void PathFieldView::paintEvent(QPaintEvent* event)
			{
				(void)event;

				//
				// Draw the field
				//
				QPainter paint(this);
				doPaint(paint);
			}

			void PathFieldView::doPaint(QPainter& paint, bool printing)
			{
				QRectF rect(0.0f, 0.0f, field_image_.width() * image_scale_, field_image_.height() * image_scale_);
				paint.drawImage(rect, field_image_);

				for(int i = 0 ; i < tracks_.size() ; i++)
					paintTrack(paint, tracks_[i], i);
			}

			void PathFieldView::emitMouseMoved(Translation2d pos)
			{
				emit mouseMoved(pos);
			}

			void PathFieldView::mouseMoveEvent(QMouseEvent* ev)
			{
			}

			void PathFieldView::mousePressEvent(QMouseEvent* ev)
			{
			}

			void PathFieldView::mouseReleaseEvent(QMouseEvent* ev)
			{
			}

			void PathFieldView::keyPressEvent(QKeyEvent* ev)
			{
			}

			std::vector<QPointF> PathFieldView::transformPoints(QTransform& trans, const std::vector<QPointF>& points)
			{
				std::vector<QPointF> result;

				for (const QPointF& pt : points)
				{
					QPointF newpt = trans.map(pt);
					result.push_back(newpt);
				}

				return result;
			}

			void PathFieldView::resizeEvent(QResizeEvent* event)
			{
				(void)event;
				if (field_ != nullptr)
					createTransforms();
			}

			void PathFieldView::setField(std::shared_ptr<GameField> field)
			{
				field_ = field;

				//
				// Load the image,
				//
				QImage image(field_->getImageFile());
				field_image_ = image;

				//
				// Create new transforms
				//
				createTransforms();

				//
				// Redraw the widget
				//
				repaint(geometry());
			}

			void PathFieldView::createTransforms()
			{
				//
				// First calculate the scale to use to scale the image to the window
				// without changing the aspec ratio.  Pick the largest scale factor that will fit
				// the most constrained dimension
				//
				double sx = (double)width() / (double)field_image_.width();
				double sy = (double)height() / (double)field_image_.height();

				if (sx < sy)
					image_scale_ = sx;
				else
					image_scale_ = sy;

				//
				// Now, we know now to scale the image to window pixels, scale the top left and bottom
				// right coordiantes from the image to the window
				//
				Translation2d tl = field_->getTopLeft().scale(image_scale_);
				Translation2d br = field_->getBottomRight().scale(image_scale_);

				//
				// Now we know two points on the field and how they coorespond to two points on the window.
				// The field point (0, FieldSize.Y) cooresponds to the point tl.
				// The field point (FieldsSize.X, 0) coorespons to the point br.
				//
				// Compute the transform based on this.  
				//
				world_to_window_ = QTransform();

				world_to_window_.translate(tl.getX(), br.getY());
				double scale = (br.getX() - tl.getX()) / field_->getSize().getX();
				world_to_window_.scale(scale, -scale);

				window_to_world_ = world_to_window_.inverted();
			}

			QPointF PathFieldView::worldToWindow(const QPointF& pt)
			{
				return world_to_window_.map(pt);
			}

			QPointF PathFieldView::windowToWorld(const QPointF& pt)
			{
				return window_to_world_.map(pt);
			}

			std::vector<QPointF> PathFieldView::worldToWindow(const std::vector<QPointF>& points)
			{
				return transformPoints(world_to_window_, points);
			}

			std::vector<QPointF> PathFieldView::windowToWorld(const std::vector<QPointF>& points)
			{
				return transformPoints(window_to_world_, points);
			}

			void PathFieldView::paintTrack(QPainter& paint, std::shared_ptr<RobotTrack> t, int index)
			{
				QPen pen(t->color());
				pen.setWidth(3);
				paint.save();
				paint.setPen(pen);
				int stpt = -1, enpt = -1;

				for (int i = 1; i < t->locSize(); i++) {
					if (t->time(i) < t->start())
						continue;

					if (stpt == -1)
						stpt = i;

					xero::paths::Translation2d p1 = t->loc(i - 1);
					xero::paths::Translation2d p2 = t->loc(i);

					QPointF pf1 = worldToWindow(QPointF(p1.getX(), p1.getY()));
					QPointF pf2 = worldToWindow(QPointF(p2.getX(), p2.getY()));

					paint.drawLine(pf1, pf2);

					if (t->time(i) > t->end())
					{
						enpt = i;
						break;
					}
				}

				if (enpt == -1)
					enpt = t->locSize() - 1;

				xero::paths::Translation2d pt = t->loc(enpt);
				QPointF pf = worldToWindow(QPointF(pt.getX(), pt.getY()));
				pen.setWidth(4.0);
				paint.setPen(pen);
				paint.drawLine(pf.x() - 12.0, pf.y() - 12.0, pf.x() + 12.0, pf.y() + 12.0);
				paint.drawLine(pf.x() - 12.0, pf.y() + 12.0, pf.x() + 12.0, pf.y() - 12.0);

				pt = t->loc(stpt);
				pf = worldToWindow(QPointF(pt.getX(), pt.getY()));
				pen.setWidth(4.0);
				paint.setPen(pen);
				QBrush br(QColor(255, 255, 255, 255));
				paint.setBrush(br);
				paint.drawEllipse(pf, 18.0, 18.0);

				br = QBrush(QColor(0, 0, 0, 255));
				pen = QPen(QColor(0, 0, 0, 255));
				paint.setBrush(br);
				paint.setPen(pen);
				QString tm = t->team().mid(3);
				QFontMetricsF fm(paint.font());
				QPointF p(pf.x() - fm.horizontalAdvance(tm) / 2, pf.y() + fm.height() / 2 - fm.descent());
				paint.drawText(p, tm);

				paint.restore();
			}
		}
	}
}
