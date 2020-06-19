#include "SequenceAnalyzer.h"
#include "SequenceEnterExitPattern.h"
#include "SequenceEnterIdlePattern.h"
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			SequenceAnalyzer::SequenceAnalyzer()
			{
			}

			SequenceAnalyzer::~SequenceAnalyzer()
			{
			}

			void SequenceAnalyzer::clearAll()
			{
			}

			void SequenceAnalyzer::analyze()
			{
				matches_.clear();
				for (auto seq : sequences_)
				{
					analyzeOneSequence(seq);
				}
			}

			void SequenceAnalyzer::analyzeOneSequence(std::shared_ptr<ZebraSequence> seq)
			{
				for (int i = 0; i < seq->size(); i++)
				{
					for (auto p : activities_)
					{
						std::pair<int, int> match = seq->matchPattern(p->patterns(), i);
						if (match.first != -1 && match.second != -1)
						{
							MatchedSequence m(p->name(), seq, match.first, match.second);
							matches_.push_back(m);
						}
					}
				}
			}

			void SequenceAnalyzer::addBlueSequences()
			{

			}
		}
	}
}
