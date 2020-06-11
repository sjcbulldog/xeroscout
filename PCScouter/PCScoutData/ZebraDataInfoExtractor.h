#pragma once

#include "pcscoutdata_global.h"
#include "ScoutingDataMap.h"
#include <QObject>
#include <memory>
#include <cmath>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class RobotTrack;

			class PCSCOUTDATA_EXPORT ZebraDataInfoExtractor
			{
			public:
				ZebraDataInfoExtractor(std::shared_ptr<RobotTrack> track);
				virtual ~ZebraDataInfoExtractor();

				ScoutingDataMapPtr characterize();

				void flush() {
					data_ = nullptr;
				}

				double window() const {
					return window_;
				}

				void setWindow(double w) {
					window_ = w;
				}

			private:

				double distance(const QPointF& start, const QPointF& end) {
					double dx = start.x() - end.x();
					double dy = start.y() - end.y();
					return std::sqrt(dx * dx + dy * dy);
				}

				double calcVelocity(double sttime, const QPointF& start, double entime, const QPointF& end) {
					return distance(start, end) / (entime - sttime);
				}

				void calcRobotCharacterstics();

				int findIndex(double time);

			private:
				double window_;
				std::shared_ptr<RobotTrack> track_;
				ScoutingDataMapPtr data_;
			};
		}
	}
}
