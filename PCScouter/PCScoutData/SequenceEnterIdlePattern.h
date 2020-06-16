#pragma once
#pragma once

#include "SequencePattern.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class SequenceEnterIdlePattern : public SequencePattern
			{
			public:
				SequenceEnterIdlePattern(const QString& name, int mincnt, int maxcnt, bool perall);
				virtual ~SequenceEnterIdlePattern();

				virtual int doesMatch(Alliance a, const ZebraSequence& sequence, int start);

			private:
				QString name_;
			};
		}
	}
}


