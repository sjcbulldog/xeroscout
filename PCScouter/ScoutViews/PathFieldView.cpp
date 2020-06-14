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
#include "RectFieldRegion.h"
#include "CircleFieldRegion.h"
#include "PolygonFieldRegion.h"
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QDebug>
#include <QCoreApplication>
#include <QFontMetricsF>
#include <QToolTip>
#include <cmath>
#include <cstdlib>

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
				heatmap_box_size_ = 1.0 / 12.0;
				text_in_heatmap_ = false;
				show_defense_ = true;
				mode_ = SelectMode::None;
			}

			PathFieldView::~PathFieldView()
			{
			}

			void PathFieldView::selectRectArea()
			{
				mode_ = SelectMode::StartingRectangular;
			}

			void PathFieldView::selectCircularArea()
			{
				mode_ = SelectMode::StartingCircular;
			}

			void PathFieldView::selectPolygonArea()
			{
				points_.clear();
				mode_ = SelectMode::StartingPolygon;
			}

			void PathFieldView::mousePressEvent(QMouseEvent* ev)
			{
				if (ev->button() == Qt::MouseButton::LeftButton)
				{
					base_pt_ = ev->pos();

					if (mode_ == SelectMode::StartingCircular)
					{
						mode_ = SelectMode::DraggingCircular;
					}
					else if (mode_ == SelectMode::StartingRectangular)
					{
						mode_ = SelectMode::DraggingRectangular;
					}
					else if (mode_ == SelectMode::StartingPolygon)
					{
						points_.push_back(ev->pos());
						mode_ = SelectMode::ContinuingPolygon;
					}
					else if (mode_ == SelectMode::ContinuingPolygon)
					{
						points_.push_back(ev->pos());

						double dist = distSquared(points_[0], points_[points_.size() - 1]);
						if (dist < 20)
						{
							std::vector<QPointF> points;

							for (const QPointF& pt : points_)
								points.push_back(windowToWorld(pt));

							mode_ = SelectMode::None;
							emit polySelected(points);

							update();
						}
					}
				}
			}

			void PathFieldView::mouseMoveEvent(QMouseEvent* ev)
			{
				current_pt_ = ev->pos();
				update();
			}

			void PathFieldView::mouseReleaseEvent(QMouseEvent* ev)
			{
				if (ev->button() == Qt::MouseButton::LeftButton)
				{
					int x = std::min(base_pt_.x(), current_pt_.x());
					int y = std::min(base_pt_.y(), current_pt_.y());
					int width = base_pt_.x() - current_pt_.x();
					if (width < 0)
						width = -width;

					int height = base_pt_.y() - current_pt_.y();
					if (height < 0)
						height = -height;

					QRectF r;
					if (mode_ == SelectMode::DraggingCircular)
					{
						double rad = std::sqrt(width * width + height * height);
						r = QRect(base_pt_.x() - rad, base_pt_.y() - rad, 2 * rad, 2 * rad);
						r = windowToWorld(r);

						mode_ = SelectMode::None;
						emit areaSelected(r);

						update();

					}
					else if (mode_ == SelectMode::DraggingRectangular)
					{
						r = QRectF(x, y, width, height);
						r = windowToWorld(r);

						mode_ = SelectMode::None;
						emit areaSelected(r);

						update();
					}
					else if (mode_ == SelectMode::ContinuingPolygon)
					{
					}
					else if (mode_ == SelectMode::None)
					{
					}
					else
					{
						assert(false);
					}
				}
			}

			void PathFieldView::contextMenuEvent(QContextMenuEvent* ev)
			{
				emit showContextMenu(ev->globalPos());
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

			void PathFieldView::paintSelectPolygon(QPainter& paint)
			{
				paint.save();
				QPen pen(QColor(192, 192, 192));
				paint.setPen(pen);
				paint.setBrush(Qt::BrushStyle::NoBrush);

				for (int i = 0; i < points_.size() - 1; i++)
				{
					paint.drawLine(points_[i], points_[i + 1]);
				}

				paint.drawLine(points_[points_.size() - 1], current_pt_);
				paint.restore();
			}

			void PathFieldView::paintSelect(QPainter& paint)
			{
				paint.save();
				QPen pen(QColor(192, 192, 192));
				paint.setPen(pen);
				paint.setBrush(Qt::BrushStyle::NoBrush);

				int width = base_pt_.x() - current_pt_.x();
				int height = base_pt_.y() - current_pt_.y();
				if (width < 0)
					width = -width;
				if (height < 0)
					height = -height;

				if (mode_ == SelectMode::DraggingRectangular)
				{
					int x = std::min(base_pt_.x(), current_pt_.x());
					int y = std::min(base_pt_.y(), current_pt_.y());
					paint.drawRect(QRect(x, y, width, height));
				}
				else
				{
					double rad = std::sqrt(width * width + height * height);
					paint.drawEllipse(base_pt_, rad, rad);

					qDebug() << "PathFieldView::paint " << base_pt_ << " radius " << rad;
				}
				paint.restore();
			}

			void PathFieldView::paintHighlights(QPainter& paint)
			{
				paint.save();
				paint.setBrush(Qt::BrushStyle::NoBrush);
				QPen pen(QColor(64, 64, 64, 255));
				paint.setPen(pen);

				for (auto h : highlights_)
				{
					std::shared_ptr<const RectFieldRegion> rregion = std::dynamic_pointer_cast<const RectFieldRegion>(h);
					if (rregion != nullptr)
					{
						paint.drawRect(worldToWindow(h->drawBounds()));
						continue;
					}

					std::shared_ptr<const CircleFieldRegion> cregion = std::dynamic_pointer_cast<const CircleFieldRegion>(h);
					if (cregion != nullptr)
					{
						paint.drawEllipse(worldToWindow(h->drawBounds()));
						continue;
					}

					std::shared_ptr<const PolygonFieldRegion> pregion = std::dynamic_pointer_cast<const PolygonFieldRegion>(h);
					{
						paint.drawPolygon(worldToWindow(pregion->polygon()), Qt::FillRule::OddEvenFill);
					}
				}
				paint.restore();
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

					paintDefense(paint);
					paintHighlights(paint);
					if (mode_ == SelectMode::DraggingCircular || mode_ == SelectMode::DraggingRectangular)
						paintSelect(paint);
					else if (mode_ == SelectMode::ContinuingPolygon)
						paintSelectPolygon(paint);
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
				int incr = static_cast<int>(1.0 / heatmap_box_size_);
				int xboxes = static_cast<int>((field_->getSize().x() + heatmap_box_size_) / heatmap_box_size_) + 1;
				int yboxes = static_cast<int>((field_->getSize().y() + heatmap_box_size_) / heatmap_box_size_) + 1;
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
				int px, py;
				for (auto tr : tracks_)
				{
					if (tr->hasData())
					{
						for (double tm = tr->start(); tm < tr->end(); tm += 0.1)
						{
							QPointF loc = tr->point(tm);
							QPoint box = pointToHeatmapBox(loc);

							for (int x = box.x() - incr; x < box.x() + incr; x++)
							{
								for (int y = box.y() - incr; y < box.y() + incr; y++)
								{
									if (x >= 0 && y >= 0 && y < yboxes && x < xboxes)
									{
										int n = (heatmap[y])[x] + 1;
										if (n > maxv)
											maxv = n;

										(heatmap[y])[x] = n;
									}
								}
							}

						}
					}
				}

				//
				// Draw the heatmap
				//
				paint.setPen(Qt::PenStyle::NoPen);
				QFont font = paint.font();
				font.setPointSizeF(4.0);
				QFontMetricsF fm(font);
				paint.setFont(font);
				QPen p(QColor(255, 255, 255));
				for (int x = 0; x < xboxes; x++)
				{
					for (int y = 0; y < yboxes; y++)
					{
						int n = (heatmap[y])[x];
						if (n != 0)
						{
							double rel = (double)n / (double)maxv;

							QPointF p1 = worldToWindow(QPointF(x * heatmap_box_size_, y * heatmap_box_size_));
							QPointF p2 = worldToWindow(QPointF((x + 1) * heatmap_box_size_, (y + 1) * heatmap_box_size_));
							QRectF r(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::abs(p1.x() - p2.x()), std::abs(p1.y() - p2.y()));

							paint.setPen(Qt::PenStyle::NoPen);
							QBrush br(heatmapColorGenerator(rel));
							paint.setBrush(br);
							paint.drawRect(r);

							if (text_in_heatmap_)
							{
								paint.setPen(p);
								QString str = QString::number(n);
								QPoint pt = QPoint(r.center().x() - fm.horizontalAdvance(str) / 2, r.center().y() + fm.descent());
								paint.drawText(pt, str);
							}
						}
					}
				}
			}

			QColor PathFieldView::heatmapColorGenerator(double rel)
			{
				static std::vector<QColor> colors =
				{
					QColor(0, 0, 255),
					QColor(0, 255, 255),
					QColor(0, 255, 0),
					QColor(255, 255, 0),
					QColor(255, 0, 0)
				};

				int index = (rel * colors.size() - 1);
				if (index >= colors.size() - 1)
					return colors[colors.size() - 1];

				double step = 1.0 / (double)(colors.size() - 1);
				double partial = rel - index * step;

				double r = colors[index].red() + (colors[index + 1].red() - colors[index].red()) * partial;
				double g = colors[index].green() + (colors[index + 1].green() - colors[index].green()) * partial;
				double b = colors[index].blue() + (colors[index + 1].blue() - colors[index].blue()) * partial;

				return QColor(r, g, b, 64);
			}

			void PathFieldView::paintRobot(QPainter& paint, std::shared_ptr<RobotTrack> track)
			{
				if (track->hasData())
				{
					QPointF pt = track->point(track->current());
					paintRobotID(paint, pt, track);
				}
			}

			void PathFieldView::paintRectHighlight(QPainter& paint, QColor c, const QRectF& r, const QString &title)
			{
				paint.save();
				QPen pen(QColor(0, 0, 0, 255));
				paint.setPen(pen);
				QBrush brush(c, Qt::BrushStyle::Dense6Pattern);
				paint.setBrush(brush);

				QRectF rp = worldToWindow(r);
				paint.drawRect(rp);

				if (title.length() > 0)
				{
					QPen pen(QColor(242, 245, 66));
					paint.setPen(pen);
					QFontMetricsF fm(paint.font());
					QPointF pt(rp.center().x() - fm.horizontalAdvance(title) / 2, rp.center().y() + fm.descent());
					paint.drawText(pt, title);
				}

				paint.restore();
			}

			void PathFieldView::paintPolygonHighlight(QPainter& paint, QColor c, const QPolygonF& polygon, const QString& title)
			{
				paint.save();
				QPen pen(QColor(0, 0, 0, 255));
				paint.setPen(pen);
				QBrush brush(c, Qt::BrushStyle::Dense6Pattern);
				paint.setBrush(brush);

				QPolygonF poly = worldToWindow(polygon);
				QRectF rp = poly.boundingRect();

				paint.drawPolygon(poly, Qt::FillRule::OddEvenFill);

				if (title.length() > 0)
				{
					QPen pen(QColor(242, 245, 66));
					paint.setPen(pen);
					QFontMetricsF fm(paint.font());
					QPointF pt(rp.center().x() - fm.horizontalAdvance(title) / 2, rp.center().y() + fm.descent());
					paint.drawText(pt, title);
				}

				paint.restore();
			}

			void PathFieldView::paintCircleHighlight(QPainter& paint, QColor c, const QRectF &r, const QString &title)
			{
				paint.save();
				QPen pen(QColor(0, 0, 0, 255));
				paint.setPen(pen);
				QBrush brush(c, Qt::BrushStyle::Dense6Pattern);
				paint.setBrush(brush);

				QRectF rp = worldToWindow(r);
				paint.drawEllipse(rp);

				if (title.length() > 0)
				{
					QPen pen(QColor(242, 245, 66));
					paint.setPen(pen);
					QFontMetricsF fm(paint.font());
					QPointF pt(rp.center().x() - fm.horizontalAdvance(title) / 2, rp.center().y() + fm.descent());
					paint.drawText(pt, title);
				}

				paint.restore();
			}

			double PathFieldView::distSquared(const QPointF& r1, const QPointF& r2)
			{
				double dx = r1.x() - r2.x();
				double dy = r1.y() - r2.y();

				return dx * dx + dy * dy;
			}

			void PathFieldView::paintDefense(QPainter& paint)
			{
				QRectF bounds;

				for (int i = 0; i < tracks_.size(); i++)
				{
					QPointF r1 = tracks_[i]->point(tracks_[i]->current());

					for (auto h : highlights_)
					{
						if (h->doesAllianceMatch(tracks_[i]->alliance()) && h->isWithin(r1))
						{
							std::shared_ptr<const RectFieldRegion> rregion = std::dynamic_pointer_cast<const RectFieldRegion>(h);
							if (rregion != nullptr)
							{
								paintRectHighlight(paint, h->color(), h->drawBounds(), h->name());
								continue;
							}

							std::shared_ptr<const CircleFieldRegion> cregion = std::dynamic_pointer_cast<const CircleFieldRegion>(h);
							if (cregion != nullptr)
							{
								paintCircleHighlight(paint, h->color(), h->drawBounds(), h->name());
								continue;
							}

							std::shared_ptr<const PolygonFieldRegion> pregion = std::dynamic_pointer_cast<const PolygonFieldRegion>(h);
							if (pregion != nullptr)
							{
								paintPolygonHighlight(paint, h->color(), pregion->polygon(), h->name());
								continue;
							}

							assert(false);
						}
					}

					if (show_defense_)
					{
						for (int j = 0; j < tracks_.size(); j++)
						{
							if (i != j)
							{
								QPointF r2 = tracks_[j]->point(tracks_[j]->current());

								if (distSquared(r1, r2) < 24)
								{
									if (tracks_[i]->alliance() != tracks_[j]->alliance())
									{
										bounds = QRectF(r1.x() - 2.0, r1.y() - 2.0, 4.0, 4.0);
										paintCircleHighlight(paint, QColor(0, 255, 0, 128), bounds, "defense");

										bounds = QRectF(r2.x() - 2.0, r2.y() - 2.0, 4.0, 4.0);
										paintCircleHighlight(paint, QColor(0, 255, 0, 128), bounds, "defense");
									}
								}
							}
						}
					}
				}
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

			QRectF PathFieldView::worldToWindow(const QRectF& r)
			{
				QPointF p1 = worldToWindow(r.topLeft());
				QPointF p2 = worldToWindow(r.bottomRight());

				double x = std::min(p1.x(), p2.x());
				double y = std::min(p1.y(), p2.y());

				double width = p2.x() - p1.x();
				if (width < 0)
					width = -width;

				double height = p2.y() - p1.y();
				if (height < 0)
					height = -height;

				return QRectF(x, y, width, height);
			}

			QRectF PathFieldView::windowToWorld(const QRectF& r)
			{
				QPointF p1 = windowToWorld(r.topLeft());
				QPointF p2 = windowToWorld(r.bottomRight());

				double x = std::min(p1.x(), p2.x());
				double y = std::min(p1.y(), p2.y());

				double width = p2.x() - p1.x();
				if (width < 0)
					width = -width;

				double height = p2.y() - p1.y();
				if (height < 0)
					height = -height;

				return QRectF(x, y, width, height);
			}

			QPolygonF PathFieldView::worldToWindow(const QPolygonF& poly)
			{
				QVector<QPointF> points;

				for (const QPointF& pt : poly)
					points.push_back(worldToWindow(pt));

				return QPolygonF(points);
			}

			QPolygonF PathFieldView::windowToWorld(const QPolygonF& poly)
			{
				QVector<QPointF> points;

				for (const QPointF& pt : poly)
					points.push_back(windowToWorld(pt));

				return QPolygonF(points);
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
				QFontMetricsF fm(paint.font());
				QPointF p(pf.x() - fm.horizontalAdvance(t->title()) / 2, pf.y() + fm.height() / 2 - fm.descent());
				paint.drawText(p, t->title());
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
