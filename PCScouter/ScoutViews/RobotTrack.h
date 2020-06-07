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
                RobotTrack(const QString& team, QColor c, const QString &match) {
                    team_ = team;
                    color_ = c;
                    match_ = match;
                }

                const QString& match() const {
                    return match_;
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

                xero::paths::Translation2d beginning() {
                    for (int i = 0; i < time_.size() - 1; i++)
                    {
                        if (time_[i] < range_start_ && time_[i + 1] >= range_start_)
                            return points_[i];
                    }

                    return xero::paths::Translation2d(0, 0);
                }

            private:
                QString team_;
                QColor color_;
                QString match_;
                std::vector<double> time_;
                std::vector<xero::paths::Translation2d> points_;
                double range_start_;
                double range_end_;
            };
        }
    }
}


