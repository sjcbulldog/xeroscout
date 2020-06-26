#include "SequenceAnalyzer.h"
#include "SequenceEnterExitPattern.h"
#include "SequenceEnterIdlePattern.h"
#include "DataModelBuilder.h"
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

			double SequenceAnalyzer::findShortestIdle(std::shared_ptr<const RobotActivity> activity)
			{
				double minv = std::numeric_limits<double>::max();

				for (auto p : activity->patterns())
				{
					std::shared_ptr<const SequenceEnterIdlePattern> idle = std::dynamic_pointer_cast<const SequenceEnterIdlePattern>(p);
					if (idle != nullptr && idle->duration() < minv)
						minv = idle->duration();
				}

				return minv;
			}

			double SequenceAnalyzer::findShortestIdle()
			{
				double minv = std::numeric_limits<double>::max();
				for (auto act : activities_)
				{
					double thisv = findShortestIdle(act);
					if (thisv < minv)
						minv = thisv;
				}

				return minv;
			}

			void SequenceAnalyzer::analyze(std::list<std::pair<QString, QString>>& which)
			{
				double idle = findShortestIdle();
				std::vector<std::shared_ptr<ZebraSequence>> sequences;

				//
				// Create Sequences
				//
				for (const std::pair<QString, QString>& pair : which)
				{
					Alliance c;
					int slot;

					auto m = dm_->findMatchByKey(pair.first);
					if (!m->hasZebra())
						continue;

					auto t = dm_->findTeamByKey(pair.second);

					if (!m->teamToAllianceSlot(t->key(), c, slot))
						continue;

					auto track = DataModelBuilder::createTrack(dm_, m->key(), t->key());
					if (track == nullptr && !track->hasData())
						continue;

					std::shared_ptr<ZebraSequence> seq = std::make_shared<ZebraSequence>(m->key(), t->key(), track, dm_->fieldRegions(), idle);
					sequences.push_back(seq);
				}

				matches_.clear();
				for(auto seq : sequences)
					analyzeOneSequence(seq);
			}

			bool SequenceAnalyzer::isDuplicate(const MatchedSequence& m)
			{
				for (const MatchedSequence& one : matches_)
				{
					if (one.name() != m.name())
						continue;

					if (one.end() == m.end())
						return true;
				}

				return false;
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
							//
							// We have a match, make sure its not a duplicate
							//

							MatchedSequence m(p->name(), seq, match.first, match.second);
							if (!isDuplicate(m))
								matches_.push_back(m);
						}
					}
				}
			}
		}
	}
}
