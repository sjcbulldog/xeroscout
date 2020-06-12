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
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>
#include <QCoreApplication>
#include <QFontMetricsF>
#include <QToolTip>
#include <cmath>

using namespace xero::scouting::datamodel;

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
				heatmap_box_size_ = 0.5;
			}

			PathFieldView::~PathFieldView()
			{
			}

			bool PathFieldView::event(QEvent* ev)
			{
				if (ev->type() == QEvent::ToolTip)
				{
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

			void PathFieldView::doPaint(QPainter& paint)
			{
				QRectF rect(0.0f, 0.0f, field_image_.width() * image_scale_, field_image_.height() * image_scale_);
				paint.drawImage(rect, field_image_);

				if (view_mode_ == ViewMode::Track)
				{
					for (int i = 0; i < tracks_.size(); i++)
						paintTrack(paint, tracks_[i]);
				}
				else if (view_mode_ == ViewMode::Robot)
				{
					for (int i = 0; i < tracks_.size(); i++)
						paintRobot(paint, tracks_[i]);
				}
				else
				{
					paintHeatmap(paint);
				}
			}

			QPoint PathFieldView::pointToHeatmapBox(const QPointF& pt)
			{
				int x = static_cast<int>(pt.x() / heatmap_box_size_);
				int y = static_cast<int>(pt.y() / heatmap_box_size_);

				return QPoint(x, y);
			}

			void PathFieldView::paintHeatmap(QPainter& paint)
			{
				// 
				// Initialize the heatmap
				//
				int xboxes = static_cast<int>(field_->getSize().x() / heatmap_box_size_) + 1;
				int yboxes = static_cast<int>(field_->getSize().x() / heatmap_box_size_) + 1;
				std::vector<std::vector<int>> heatmap;
				for (int y = 0; y < yboxes; y++)
				{
					std::vector<int> row(xboxes);
					std::fill(row.begin(), row.end(), 0);
					heatmap.push_back(row);
				}

				//
				// Calculate the heatmap
				//
				int maxv = 0;
				for (auto tr : tracks_)
				{
					for (double tm = tr->start(); tm < tr->end(); tm += 0.5)
					{
						QPointF loc = tr->point(tm);
						QPoint box = pointToHeatmapBox(loc);

						int n = (heatmap[box.y()])[box.x()] + 1;
						if (n > maxv)
							maxv = n;

						(heatmap[box.y()])[box.x()] = n;
					}
				}

				//
				// Draw the heatmap
				//
				paint.setPen(Qt::PenStyle::NoPen);
				for (int x = 0; x < xboxes; x++)
				{
					for (int y = 0; y < yboxes; y++)
					{
						int n = (heatmap[x])[y];
						double rel = (double)n / (double)maxv;

						QRectF sq(x * heatmap_box_size_, y * heatmap_box_size_, heatmap_box_size_, heatmap_box_size_);
						int red = static_cast<int>(rel / 2.0 + 0.5 * 255);
						QColor c(red, 0, 0, 128);
						QBrush br(c);
						paint.setBrush(br);
					}
				}
			}

			void PathFieldView::paintRobot(QPainter& paint, std::shared_ptr<RobotTrack> track)
			{
				if (track->hasData())
				{
					QPointF pt = track->point(track->current());
					paintRobotID(paint, pt, track);
				}
			}


			void PathFieldView::emitMouseMoved(QPointF pos)
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
				QPointF tl = image_scale_ * field_->getTopLeft();
				QPointF br = image_scale_ * field_->getBottomRight();

				//
				// Now we know two points on the field and how they coorespond to two points on the window.
				// The field point (0, FieldSize.Y) cooresponds to the point tl.
				// The field point (FieldsSize.X, 0) coorespons to the point br.
				//
				// Compute the transform based on this.  
				//
				world_to_window_ = QTransform();

				world_to_window_.translate(tl.x(), br.y());
				double scale = (br.x() - tl.x()) / field_->getSize().x();
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

			void PathFieldView::paintRobotID(QPainter& paint, const QPointF& pt, std::shared_ptr<RobotTrack> t)
			{
				QPointF pf = worldToWindow(QPointF(pt.x(), pt.y()));
				QPen pen(t->color());
				pen.setWidth(4.0);
				paint.setPen(pen);
				QBrush br(QColor(255, 255, 255, 255));
				paint.setBrush(br);
				paint.drawEllipse(pf, 20.0, 20.0);

				br = QBrush(QColor(0, 0, 0, 255));
				pen = QPen(QColor(0, 0, 0, 255));
				paint.setBrush(br);
				paint.setPen(pen);
				QString tm = QString::number(t->teamNumber());
				QFontMetricsF fm(paint.font());
				QPointF p(pf.x() - fm.horizontalAdvance(tm) / 2, pf.y() + fm.height() / 2 - fm.descent());
				paint.drawText(p, tm);
			}

			void PathFieldView::paintTrack(QPainter& paint, std::shared_ptr<RobotTrack> t)
			{
				if (t->pointCount() == 0)
					return;

				QPen pen(t->color());
				pen.setWidth(3);
				paint.save();
				paint.setPen(pen);
				int stpt = -1, enpt = -1;

				for (int i = 1; i < t->pointCount(); i++) {
					if (t->time(i) < t->start())
						continue;

					if (stpt == -1)
						stpt = i;

					QPointF p1 = t->point(i - 1);
					QPointF p2 = t->point(i);

					QPointF pf1 = worldToWindow(QPointF(p1.x(), p1.y()));
					QPointF pf2 = worldToWindow(QPointF(p2.x(), p2.y()));

					paint.drawLine(pf1, pf2);

					if (t->time(i) > t->end())
					{
						enpt = i;
						break;
					}
				}

				if (enpt == -1)
					enpt = t->pointCount() - 1;

				QPointF pt = t->point(enpt);
				QPointF pf = worldToWindow(QPointF(pt.x(), pt.y()));
				pen.setWidth(4.0);
				paint.setPen(pen);
				paint.drawLine(pf.x() - 12.0, pf.y() - 12.0, pf.x() + 12.0, pf.y() + 12.0);
				paint.drawLine(pf.x() - 12.0, pf.y() + 12.0, pf.x() + 12.0, pf.y() - 12.0);

				paintRobotID(paint, t->point(stpt), t);

				paint.restore();
			}
		}
	}
}
