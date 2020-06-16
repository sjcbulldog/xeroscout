#include "SequenceEnterIdlePattern.h"
#include "ZebraEvent.h"
#include "ZebraSequence.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			SequenceEnterIdlePattern::SequenceEnterIdlePattern(const QString& name, int mincnt, int maxcnt, bool perall) : SequencePattern(mincnt, maxcnt, perall)
			{
				name_ = name;
			}

			SequenceEnterIdlePattern::~SequenceEnterIdlePattern()
			{
			}

			int SequenceEnterIdlePattern::doesMatch(Alliance a, const ZebraSequence& sequence, int start)
			{
				assert(perAlliance() == false || a == Alliance::Red || a == Alliance::Blue);

				QString name = name_;
				if (perAlliance())
					name = toString(a) + "-" + name_;

				if (sequence[start].isEnter() && sequence[start].name() == name &&
					sequence[start + 1].isIdle())
					return 2;

				return 0;
			}
		}
	}
}
