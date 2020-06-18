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
				auto act = std::make_shared<RobotActivity>("Load-Trench-Trenchshot");
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("ControlPanel", 1, 1, true));
				act->addPattern(std::make_shared<SequenceEnterIdlePattern>("Trench", 1, 1, true));
				activities_.push_back(act);

				act = std::make_shared<RobotActivity>("Load-Trench-FarShot");
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("ControlPanel", 1, 1, true));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("Trench", 1, 1, true));
				act->addPattern(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));

				act = std::make_shared<RobotActivity>("Load-Central-FarShot");
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("Center", 1, 1, false));
				act->addPattern(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));

				act = std::make_shared<RobotActivity>("Load-Central-Trench");
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("LoadingStation", 1, 8, true));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("Center", 1, 1, false));
				act->addPattern(std::make_shared<SequenceEnterExitPattern>("Trench", 1, 1, true));
				act->addPattern(std::make_shared<SequenceEnterIdlePattern>("FarShot", 1, 1, true));
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
