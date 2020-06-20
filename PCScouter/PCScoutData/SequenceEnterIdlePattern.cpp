#include "SequenceEnterIdlePattern.h"
#include "ZebraEvent.h"
#include "ZebraSequence.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			SequenceEnterIdlePattern::SequenceEnterIdlePattern(const QString& name, double idle, int mincnt, int maxcnt, bool perall) : SequencePattern(mincnt, maxcnt, perall)
			{
				name_ = name;
				idle_ = idle;
			}

			SequenceEnterIdlePattern::~SequenceEnterIdlePattern()
			{
			}

			int SequenceEnterIdlePattern::doesMatch(Alliance a, const ZebraSequence& sequence, int start) const
			{
				assert(perAlliance() == false || a == Alliance::Red || a == Alliance::Blue);

				QString name = name_;
				if (perAlliance())
					name = xero::scouting::datamodel::toString(a) + "-" + name_;

				if (start >= sequence.size() - 1)
					return 0;

				if (sequence[start].isEnter() && sequence[start].name() == name &&
					sequence[start + 1].isIdle() && sequence[start + 1].duration() > idle_)
				{
					return 2;
				}

				return 0;
			}
		}
	}
}
