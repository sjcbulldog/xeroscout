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
#include "Alliance.h"
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
                RobotTrack(const QString &mkey, const QString &tkey, Alliance c) {
                    mkey_ = mkey;
                    tkey_ = tkey;
                    all_ = c;
                }

                virtual ~RobotTrack() {
                }

                void invert(double width) {
                    for (int i = 0; i < points_.size(); i++) {
                        QPointF pt(points_[i].x(), width - points_[i].y());
                    }
                }

                void setColor(QColor c) {
                    color_ = c;
                }

                void setAlliance(Alliance a) {
                    all_ = a;
                }

                void setTitle(const QString& title) {
                    title_ = title;
                }

                Alliance alliance() const {
                    return all_;
                }

                bool hasData() {
                    return time_.size() > 0 && points_.size() > 0;
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

                double current() const {
                    return current_time_;
                }

                void setCurrentTime(double t) {
                    current_time_ = t;
                }

                const QString &title() const {
                    return title_;
                }

                QColor color() const {
                    return color_;
                }

                void addTime(double t) {
                    time_.push_back(t);
                }

                int timeCount() {
                    return static_cast<int>(time_.size());
                }

                double time(int index) const {
                    return time_[index];
                }

                void addPoint(const QPointF& t) {
                    points_.push_back(t);
                }

                int pointCount() {
                    return static_cast<int>(points_.size());
                }

                const QPointF& point(int index) {
                    return points_[index];
                }

                QPointF point(double time) {
                    if (time_.size() != 0 && points_.size() != 0)
                    {
                        if (time < time_[0])
                            return points_[0];
                        else if (time > time_[time_.size() - 1])
                            return points_[points_.size() - 1];
                        else
                        {
                            for (int i = 0; i < time_.size() - 1; i++)
                            {
                                if (i >= points_.size())
                                    return points_[points_.size() - 1];

                                if (time >= time_[i] && time < time_[i + 1])
                                    return points_[i];
                            }
                        }
                    }

                    return QPointF(0.0, 0.0);
                }

                QPointF beginning() {
                    for (int i = 0; i < time_.size() - 1; i++)
                    {
                        if (time_[i] < range_start_ && time_[i + 1] >= range_start_)
                            return points_[i];
                    }

                    return QPointF(0, 0);
                }

                void transform(double width, double height) {
                    for (int i = 0; i < points_.size(); i++) {
                        QPointF pt = QPointF(width - points_[i].x(), height - points_[i].y());
                        points_[i] = pt;
                    }
                }

            private:
                QString mkey_;
                QString tkey_;
                QString title_;
                QColor color_;
                std::vector<double> time_;
                std::vector<QPointF> points_;
                double range_start_;
                double range_end_;
                double current_time_;
                Alliance all_;
            };
        }
    }
}

