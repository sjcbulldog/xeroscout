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
				void mouseMoved(QPointF pos);

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
				void paintTrack(QPainter& paint, std::shared_ptr<xero::scouting::datamodel::RobotTrack> t, int index);

			private:
				void emitMouseMoved(QPointF pos);

				std::vector<QPointF> transformPoints(QTransform& trans, const std::vector<QPointF>& points);
				void createTransforms();

			private:
				QImage field_image_;
				std::shared_ptr<GameField> field_;
				double image_scale_;
				QTransform world_to_window_;
				QTransform window_to_world_;
				std::string units_;
				std::vector<std::shared_ptr<xero::scouting::datamodel::RobotTrack>> tracks_;
			};
		}
	}
}
