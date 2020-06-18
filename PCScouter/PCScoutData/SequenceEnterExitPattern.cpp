#include "SequenceEnterExitPattern.h"
#include "ZebraEvent.h"
#include "ZebraSequence.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			SequenceEnterExitPattern::SequenceEnterExitPattern(const QString& name, int mincnt, int maxcnt, bool perall) : SequencePattern(mincnt, maxcnt, perall)
			{
				name_ = name;
			}

			SequenceEnterExitPattern::~SequenceEnterExitPattern()
			{
			}

			int SequenceEnterExitPattern::doesMatch(Alliance a, const ZebraSequence& sequence, int start) const
			{
				assert(perAlliance() == false || a == Alliance::Red || a == Alliance::Blue);

				QString name = name_;
				if (perAlliance())
					name = xero::scouting::datamodel::toString(a) + "-" + name_;

				if (!sequence[start].isEnter() || sequence[start].name() != name)
					return 0;

				int index = start;
				while (true)
				{
					index++;
					if (index == sequence.size())
						return 0;

					if (sequence[index].isLeave() && sequence[index].name() == name)
						return index - start + 1;

					if (!sequence[index].isIdle())
						return 0;
				}

				return 0;
			}
		}
	}
}
