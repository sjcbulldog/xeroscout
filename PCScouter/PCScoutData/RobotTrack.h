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

#include "pcscoutdata_global.h"
#include <QPointF>
#include <QString>
#include <QColor>
#include <vector>

namespace xero
{
    namespace scouting
    {
        namespace datamodel
        {
            class PCSCOUTDATA_EXPORT RobotTrack
            {
            public:
                RobotTrack(int number, QColor c) {
                    number_ = number;
                    color_ = c;
                }

                virtual ~RobotTrack() {
                }

                void setRange(double minv, double maxv) {
                    range_start_ = minv;
                    range_end_ = maxv;
                }

                double start() const {
                    return range_start_;
                }

                double end() const {
                    return range_end_;
                }

                int teamNumber() const {
                    return number_;
                }

                QColor color() const {
                    return color_;
                }

                void addTime(double t) {
                    time_.push_back(t);
                }

                int timeCount() {
                    return time_.size();
                }

                double time(int index) const {
                    return time_[index];
                }

                void addPoint(const QPointF& t) {
                    points_.push_back(t);
                }

                int pointCount() {
                    return points_.size();
                }

                const QPointF& point(int index) {
                    return points_[index];
                }

                void removeLastPoint() {
                    if (time_.size() == points_.size()) {
                        time_.resize(time_.size() - 1);
                        points_.resize(points_.size() - 1);
                    }
                }

                QPointF beginning() {
                    for (int i = 0; i < time_.size() - 1; i++)
                    {
                        if (time_[i] < range_start_ && time_[i + 1] >= range_start_)
                            return points_[i];
                    }

                    return QPointF(0, 0);
                }

            private:
                int number_;
                QColor color_;
                std::vector<double> time_;
                std::vector<QPointF> points_;
                double range_start_;
                double range_end_;
            };
        }
    }
}


