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
				std::vector<std::shared_ptr<SequencePattern>> pattern;

				pattern.clear();
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("ControlPanel", 1, 1, true));
				pattern.push_back(std::make_shared<SequenceEnterIdlePattern>("Trench", 1, 1, true));
				named_patterns_.insert_or_assign("Load-Trench-TrenchShot", pattern);

				pattern.clear();
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("ControlPanel", 1, 1, true));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("Trench", 1, 1, true));
				pattern.push_back(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));
				named_patterns_.insert_or_assign("Load-Trench-FarShot", pattern);

				pattern.clear();
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("Center", 1, 1, false));
				pattern.push_back(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));
				named_patterns_.insert_or_assign("Load-Central-FarShot", pattern);

				pattern.clear();
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("Center", 1, 1, false));
				pattern.push_back(std::make_shared<SequenceEnterExitPattern>("Trench", 1, 1, true));
				pattern.push_back(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));
				named_patterns_.insert_or_assign("Load-Central-Trench", pattern);
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
					for (auto p : named_patterns_)
					{
						std::pair<int, int> match = seq->matchPattern(p.second, i);
						if (match.first != -1 && match.second != -1)
						{
							MatchedSequence m(p.first, seq, match.first, match.second);
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
