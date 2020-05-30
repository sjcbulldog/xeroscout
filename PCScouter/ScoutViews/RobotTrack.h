#pragma once

#include "Translation2d.h"
#include <QString>
#include <QColor>
#include <vector>

namespace xero
{
    namespace scouting
    {
        namespace views
        {
            class RobotTrack
            {
            public:
                RobotTrack(const QString& team, QColor c) {
                    team_ = team;
                    color_ = c;
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

                const QString& team() const {
                    return team_;
                }

                QColor color() const {
                    return color_;
                }

                void addTime(double t) {
                    time_.push_back(t);
                }

                int timeSize() {
                    return time_.size();
                }

                double time(int index) const {
                    return time_[index];
                }

                void addPoint(const xero::paths::Translation2d& t) {
                    points_.push_back(t);
                }

                int locSize() {
                    return points_.size();
                }

                const xero::paths::Translation2d& loc(int index) {
                    return points_[index];
                }

                void removeLastPoint() {
                    if (time_.size() == points_.size()) {
                        time_.resize(time_.size() - 1);
                        points_.resize(points_.size() - 1);
                    }
                }

            private:
                QString team_;
                QColor color_;
                std::vector<double> time_;
                std::vector<xero::paths::Translation2d> points_;
                double range_start_;
                double range_end_;
            };
        }
    }
}


