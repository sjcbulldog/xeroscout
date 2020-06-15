#pragma once

#include "pcscoutdata_global.h"
#include "ZebraSequence.h"
#include <memory>
#include <list>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT SequenceAnalyzer
			{
			public:
				SequenceAnalyzer();
				virtual ~SequenceAnalyzer();

				void setSequences(const std::list<std::shared_ptr<ZebraSequence>>& sequences) {
					sequences_ = sequences;
					clearAll();
				}

				void clearAll();

			private:
				std::list<std::shared_ptr<ZebraSequence>> sequences_;
			};
		}
	}
}
