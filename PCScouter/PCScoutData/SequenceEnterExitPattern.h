#pragma once

#include "pcscoutdata_global.h"
#include "SequencePattern.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT SequenceEnterExitPattern : public SequencePattern
			{
			public:
				SequenceEnterExitPattern(const QString& name, int mincnt, int maxcnt, bool perall);
				virtual ~SequenceEnterExitPattern();

				virtual int doesMatch(Alliance a, const ZebraSequence& sequence, int start) const ;

				const QString& name() const {
					return name_;
				}

				QString toString() const {
					return "EnterExit:" + name_;
				}

			private:
				QString name_;
			};
		}
	}
}
