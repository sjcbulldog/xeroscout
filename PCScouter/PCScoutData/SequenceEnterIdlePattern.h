#pragma once
#pragma once

#include "pcscoutdata_global.h"
#include "SequencePattern.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT SequenceEnterIdlePattern : public SequencePattern
			{
			public:
				SequenceEnterIdlePattern(const QString& name, int mincnt, int maxcnt, bool perall);
				virtual ~SequenceEnterIdlePattern();

				virtual int doesMatch(Alliance a, const ZebraSequence& sequence, int start) const;

				const QString& name() const {
					return name_;
				}

				QString toString() const {
					return "EnterIdle:" + name_;
				}

			private:
				QString name_;
			};
		}
	}
}


