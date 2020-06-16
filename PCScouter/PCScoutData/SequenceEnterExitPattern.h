#pragma once

#include "SequencePattern.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class SequenceEnterExitPattern : public SequencePattern
			{
			public:
				SequenceEnterExitPattern(const QString& name, int mincnt, int maxcnt, bool perall);
				virtual ~SequenceEnterExitPattern();

				virtual int doesMatch(Alliance a, const ZebraSequence& sequence, int start);

			private:
				QString name_;
			};
		}
	}
}
