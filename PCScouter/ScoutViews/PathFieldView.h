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

#pragma once

#include "GameField.h"
#include "ViewBase.h"
#include "RobotTrack.h"
#include "FieldRegion.h"
#include <QColor>
#include <QWidget>
#include <QPixmap>
#include <QTransform>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PathFieldView : public QWidget
			{
				friend class ShowRobotWindow;

				Q_OBJECT

			public:
				enum class ViewMode
				{
					Heatmap,
					Track,
					Robot,
					Editor
				};

			public:
				PathFieldView(QWidget* parent = Q_NULLPTR);
				virtual ~PathFieldView();

				std::list<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> selectedRegions() {
					return selected_;
				}

				bool isOneSelected() {
					return selected_.size() == 1;
				}

				void setViewMode(ViewMode m) { view_mode_ = m; update(); }
				ViewMode viewMode() const { return view_mode_;  }

				void setField(std::shared_ptr<GameField> field);
				void setUnits(const std::string& units);

				void setShowDefense(bool b) {
					show_defense_ = b;
				}

				bool showDefense() const {
					return show_defense_;
				}

				QPointF worldToWindow(const QPointF& pt);
				QPointF windowToWorld(const QPointF& pt);
				QRectF worldToWindow(const QRectF& rect);
				QRectF windowToWorld(const QRectF& rect);
				QPolygonF worldToWindow(const QPolygonF& poly);
				QPolygonF windowToWorld(const QPolygonF& poly);

				std::vector<QPointF> worldToWindow(const std::vector<QPointF>& points);
				std::vector<QPointF> windowToWorld(const std::vector<QPointF>& points);

				QPointF globalToWorld(const QPoint& pt) {
					return windowToWorld(mapFromGlobal(pt));
				}

				void selectRectArea();
				void selectCircularArea();
				void selectPolygonArea();

				void doPaint(QPainter& paint);

				void setTextInHeatmap(bool b) {
					text_in_heatmap_ = b;
				}

				void addHighlight(std::shared_ptr<const xero::scouting::datamodel::FieldRegion> h) {
					highlights_.push_back(h);
				}

				void removeHighlight(std::shared_ptr<const xero::scouting::datamodel::FieldRegion> h) {
					highlights_.remove(h);
					selected_.remove(h);
				}

				void clearTracks() {
					tracks_.clear();
				}

				void addTrack(std::shared_ptr<xero::scouting::datamodel::RobotTrack> track) {
					tracks_.push_back(track);
					update();
				}

				void removeTrack(std::shared_ptr<xero::scouting::datamodel::RobotTrack> track) {
					auto it = std::find(tracks_.begin(), tracks_.end(), track);
					if (it != tracks_.end())
						tracks_.erase(it);
				}

				std::vector<std::shared_ptr<xero::scouting::datamodel::RobotTrack>>& tracks() {
					return tracks_;
				}

			signals:
				void showContextMenu(QPoint pt);
				void keyPressed(Qt::Key key);
				void areaSelected(const QRectF& bounds);
				void polySelected(const std::vector<QPointF>& points);

			protected:
				void paintEvent(QPaintEvent* event) override;
				void resizeEvent(QResizeEvent* event) override;
				void contextMenuEvent(QContextMenuEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void mouseMoveEvent(QMouseEvent* ev) override;
				void mouseReleaseEvent(QMouseEvent* ev) override;
				void keyPressEvent(QKeyEvent* ev) override;

				QSize minimumSizeHint() const override;
				QSize sizeHint() const override;

			private:
				enum class SelectMode
				{
					None,
					StartingCircular,
					DraggingCircular,

					StartingRectangular,
					DraggingRectangular,

					StartingPolygon,
					ContinuingPolygon,
				};

			private:
				void paintTrack(QPainter& paint, std::shared_ptr<xero::scouting::datamodel::RobotTrack> t);
				void paintRobot(QPainter& paint, std::shared_ptr<xero::scouting::datamodel::RobotTrack> t);
				void paintHeatmap(QPainter& paint);
				void paintRobotID(QPainter& paint, const QPointF& loc, std::shared_ptr<xero::scouting::datamodel::RobotTrack> t);
				void paintDefense(QPainter& paint);
				void paintSelect(QPainter& paint);
				void paintSelectPolygon(QPainter& paint);
				void paintHighlights(QPainter& paint, bool full = true);

				void paintRectHighlight(QPainter& paint, QColor c, const QRectF& bounds, const QString& title);
				void paintCircleHighlight(QPainter& paint, QColor c, const QRectF& bounds, const QString& title);
				void paintPolygonHighlight(QPainter& paint, QColor c, const QPolygonF &polygon, const QString& title);

				void drawHandle(QPainter& paint, const QPointF& pt);


				QColor heatmapColorGenerator(double rel);
				QPoint pointToHeatmapBox(const QPointF& pt);

				void selectShape(const QPointF& pt, bool add);

			private:
				std::vector<QPointF> transformPoints(QTransform& trans, const std::vector<QPointF>& points);
				void createTransforms();


			private:
				// Used only in the robot view mode
				QImage field_image_;
				std::shared_ptr<GameField> field_;
				double image_scale_;
				QTransform world_to_window_;
				QTransform window_to_world_;
				std::string units_;
				std::vector<std::shared_ptr<xero::scouting::datamodel::RobotTrack>> tracks_;
				ViewMode view_mode_;

				bool text_in_heatmap_;
				bool show_defense_;

				double heatmap_box_size_;

				QPointF base_pt_;
				QPointF current_pt_;
				SelectMode mode_;

				std::vector<QPointF> points_;

				std::list<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> highlights_;
				std::list<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> selected_;
			};
		}
	}
}
