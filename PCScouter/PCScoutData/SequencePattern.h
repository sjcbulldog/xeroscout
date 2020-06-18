#pragma once

#include "Alliance.h"
#include <QString>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ZebraSequence;

			class SequencePattern
			{
			public:
				SequencePattern(int mincnt, int maxcnt, bool perall) {
					min_count_ = mincnt;
					max_count_ = maxcnt;
					perall_ = perall;
				}

				virtual ~SequencePattern() {
				}


				bool perAlliance() const {
					return perall_;
				}

				int minCount() const {
					return min_count_;
				}

				int maxCount() const {
					return max_count_;
				}

				virtual QString toString() const = 0;
				virtual int doesMatch(Alliance a, const ZebraSequence& sequence, int start) const = 0;

			private:
				//
				// The minimum number of times to match
				//
				int min_count_;

				//
				// The maximum number of times to match
				//
				int max_count_;

				//
				// If true, this is per alliance, so any names must be prepended with
				// the allianc color
				//
				bool perall_;
			};
		}
	}
}

