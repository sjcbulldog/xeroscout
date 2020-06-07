//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 


#pragma once

#include "GameField.h"
#include "Pose2d.h"
#include "ViewBase.h"
#include "RobotTrack.h"
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
				PathFieldView(QWidget* parent = Q_NULLPTR);
				virtual ~PathFieldView();

				void setField(std::shared_ptr<GameField> field);
				void setUnits(const std::string& units);

				QPointF worldToWindow(const QPointF& pt);
				QPointF windowToWorld(const QPointF& pt);
				std::vector<QPointF> worldToWindow(const std::vector<QPointF>& points);
				std::vector<QPointF> windowToWorld(const std::vector<QPointF>& points);

				void doPaint(QPainter& paint, bool printing = false);

				void clearTracks() {
					tracks_.clear();
				}

				void addTrack(std::shared_ptr<RobotTrack> track) {
					tracks_.push_back(track);
					update();
				}

				std::vector<std::shared_ptr<RobotTrack>>& tracks() {
					return tracks_;
				}

			signals:
				void mouseMoved(xero::paths::Translation2d pos);

			protected:
				void paintEvent(QPaintEvent* event) override;
				void resizeEvent(QResizeEvent* event) override;
				void mouseMoveEvent(QMouseEvent* event) override;
				void mousePressEvent(QMouseEvent* event) override;
				void mouseReleaseEvent(QMouseEvent* event) override;
				void keyPressEvent(QKeyEvent* event) override;
				bool event(QEvent* ev);

				QSize minimumSizeHint() const override;
				QSize sizeHint() const override;

			private:
				void paintTrack(QPainter& paint, std::shared_ptr<RobotTrack> t, int index);

			private:
				void emitMouseMoved(xero::paths::Translation2d pos);

				std::vector<QPointF> transformPoints(QTransform& trans, const std::vector<QPointF>& points);
				void createTransforms();

			private:
				QImage field_image_;
				std::shared_ptr<GameField> field_;
				double image_scale_;
				QTransform world_to_window_;
				QTransform window_to_world_;
				std::string units_;
				std::vector<std::shared_ptr<RobotTrack>> tracks_;
			};
		}
	}
}
