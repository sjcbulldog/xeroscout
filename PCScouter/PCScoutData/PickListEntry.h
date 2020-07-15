#pragma once

#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PickListEntry
			{
			public:
				PickListEntry(int team, double score) {
					team_ = team;
					score_ = score;
				}

				virtual ~PickListEntry() {
				}

				void addThird(int team, double score) {
					thirds_.push_back(std::make_pair(team, score));
				}

				int team() const {
					return team_;
				}

				double score() const {
					return score_;
				}

				size_t count() const {
					return thirds_.size();
				}

				int thirdTeam(size_t index) const {
					return thirds_[index].first;
				}

				double thirdScore(size_t index) const {
					return thirds_[index].second;
				}

				void removeThird(int index) {
					auto it = thirds_.begin() + index;
					thirds_.erase(it);
				}

				void insertThird(int index, int team, double score) {
					auto it = thirds_.begin() + index;
					auto pair = std::make_pair(team, score);
					thirds_.insert(it, pair);
				}

			private:
				int team_;
				double score_;
				std::vector<std::pair<int, double>> thirds_;
			};
		}
	}
}


