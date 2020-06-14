#pragma once

#include "FieldRegion.h"
#include "RobotTrack.h"
#include "ZebraEvent.h"
#include <QString>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ZebraSequence
			{
			public:
				ZebraSequence(const QString &mkey, const QString &tkey, std::shared_ptr<RobotTrack> track,
					std::vector<std::shared_ptr<const FieldRegion>> &regions);
				virtual ~ZebraSequence();

				size_t size() const {
					return events_.size();
				}

				std::vector<ZebraEvent>::const_iterator cbegin() {
					return events_.cbegin();
				}

				std::vector<ZebraEvent>::const_iterator cend() {
					return events_.cend();
				}

			private:
				void extractEvents(std::shared_ptr<RobotTrack> track, std::vector<std::shared_ptr<const FieldRegion>>& regions);

			private:
				QString mkey_;
				QString tkey_;
				std::vector<ZebraEvent> events_;
			};
		}
	}
}
