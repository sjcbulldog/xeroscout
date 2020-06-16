#pragma once

#include "pcscoutdata_global.h"
#include "FieldRegion.h"
#include "RobotTrack.h"
#include "ZebraEvent.h"
#include "SequencePattern.h"
#include <QString>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT ZebraSequence
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

				const ZebraEvent& operator[](size_t index) const {
					return events_[index];
				}

				const QString& matchKey() const {
					return mkey_;
				}

				const QString& teamKey() const {
					return tkey_;
				}

				std::pair<int, int> matchPattern(const std::vector<std::shared_ptr<SequencePattern>>& pattern, int start);

			private:
				constexpr static const double debounceDelay = 1.0;
				constexpr static const double idleDelay = 1.5;
				constexpr static const double idleThreshold = 0.5;

			private:
				void extractEvents(std::shared_ptr<RobotTrack> track, std::vector<std::shared_ptr<const FieldRegion>>& regions);
				void debounceEvents();
				void logicallyOrder();

				void swapEvents(int i, int j);
				int findNextEvent(int i, ZebraEvent& ev);

			private:
				QString mkey_;
				QString tkey_;
				std::vector<ZebraEvent> events_;
				Alliance alliance_;
			};
		}
	}
}
