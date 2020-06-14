#include "ZebraDataInfoExtractor.h"
#include "RobotTrack.h"
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ZebraDataInfoExtractor::ZebraDataInfoExtractor(std::shared_ptr<RobotTrack> track)
			{
				window_ = 1.0;
				track_ = track;
			}

			ZebraDataInfoExtractor::~ZebraDataInfoExtractor()
			{
			}

			ScoutingDataMapPtr ZebraDataInfoExtractor::characterize()
			{
				if (data_ == nullptr)
					calcRobotCharacterstics();

				return data_;
			}

			int ZebraDataInfoExtractor::findIndex(double t)
			{
				if (std::fabs(t) < 0.01 && t <= track_->time(0))
					return 0;

				if (t >= track_->time(track_->timeCount() - 1))
					return track_->timeCount() - 1;

				for (int i = 0; i < track_->timeCount() - 1; i++) {
					if (track_->time(i) <= t && track_->time(i + 1) > t)
						return i;
				}

				return -1;
			}

			void ZebraDataInfoExtractor::calcRobotCharacterstics()
			{
				if (data_ == nullptr)
					data_ = std::make_shared<ScoutingDataMap>();
				else
					data_->clear();

				if (track_->timeCount() == 0 || track_->pointCount() == 0)
					return;

				calcMaxVelocityAccel();
			}

			void ZebraDataInfoExtractor::calcMaxVelocityAccel()
			{
				//
				// Get the index just before the start time 
				//
				int sindex = findIndex(track_->start());

				//
				// Get the index just after the end time
				//
				int eindex = findIndex(track_->end());
				if (eindex != track_->timeCount() - 1)
					eindex++;

				double maxv = 0.0;
				double maxa = 0.0;
				double lastvel = 0.0;
				while (sindex < eindex) {
					int gindex = findIndex(track_->time(sindex) + window_);
					if (gindex == -1)
						break;

					if (gindex >= eindex || gindex >= track_->pointCount())
						break;

					// Calculate the velocity between the point at sindex and the point at gindex ;
					double vel = calcVelocity(track_->time(sindex), track_->point(sindex), track_->time(gindex), track_->point(gindex));
					double accel = std::fabs((vel - lastvel) / (track_->time(gindex) - track_->time(sindex)));

					if (vel > maxv)
						maxv = vel;

					if (accel > maxa)
						maxa = accel;

					lastvel = vel;

					sindex++;
				}

				data_->insert_or_assign("Max Velocity", maxv);
				data_->insert_or_assign("Max Acceleration", maxa);
			}
		}
	}
}
