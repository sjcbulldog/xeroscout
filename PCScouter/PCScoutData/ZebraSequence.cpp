#include "ZebraSequence.h"
#include <map>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ZebraSequence::ZebraSequence(const QString& mkey, const QString& tkey, std::shared_ptr<RobotTrack> track, std::vector<std::shared_ptr<const FieldRegion>>& regions)
			{
				mkey_ = mkey;
				tkey_ = tkey;

				extractEvents(track, regions);
			}

			ZebraSequence::~ZebraSequence()
			{
			}

			void ZebraSequence::extractEvents(std::shared_ptr<RobotTrack> track, std::vector<std::shared_ptr<const FieldRegion>>& regions)
			{
				std::map<QString, bool> states;

				if (!track->hasData())
					return;

				//
				// Establish the time 0 state
				//
				for (auto region : regions)
				{
					states.insert_or_assign(region->name(), region->isWithin(track->point(0)));
				}

				//
				// Look for state changes
				for (int i = 1; i < track->timeCount(); i++)
				{
					if (i >= track->pointCount())
						break;

					for (auto region : regions)
					{
						bool b = region->isWithin(track->point(i));
						if (b != states[region->name()])
						{
							//
							// We had a state change
							//
							ZebraEvent ev(region->name(), track->time(i), b);
							events_.push_back(ev);

							states.insert_or_assign(region->name(), b);
						}
					}
				}
			}
		}
	}
}
