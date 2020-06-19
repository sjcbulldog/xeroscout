#pragma once

#include "SequencePattern.h"
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class RobotActivity
			{
			public:
				RobotActivity(const QString& name) {
					name_ = name;
				}

				const QString& name() const {
					return name_;
				}

				void setName(const QString& name) {
					name_ = name;
				}

				void addPattern(std::shared_ptr<SequencePattern> p) {
					patterns_.push_back(p);
				}

				void deletePattern(int index) {
					patterns_.erase(patterns_.begin() + index);
				}

				void setMinMaxCount(int index, int minv, int maxv) {
					patterns_[index]->setMinMax(minv, maxv);
				}

				std::vector<std::shared_ptr<const SequencePattern>> patterns() const {
					std::vector<std::shared_ptr<const SequencePattern>> list;
					for (auto p : patterns_)
						list.push_back(p);

					return list;
				}

				int count() const {
					return patterns_.size();
				}

				void movePatternUp(int index) {
					swap(index - 1, index);
				}

				void movePatternDown(int index) {
					swap(index + 1, index);
				}

			private:
				void swap(int i, int j) {
					auto tmp = patterns_[i];
					patterns_[i] = patterns_[j];
					patterns_[j] = tmp;
				}

			private:
				std::vector<std::shared_ptr<SequencePattern>> patterns_;
				QString name_;
			};
		}
	}
}

