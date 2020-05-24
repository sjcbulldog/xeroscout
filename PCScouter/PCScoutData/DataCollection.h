#pragma once

#include "pcscoutdata_global.h"
#include <QVariant>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT DataCollection
			{
			public:
				DataCollection() {
				}

				virtual ~DataCollection() {
				}

				int count() const {
					return data_.size();
				}

				QString tag(int index) const {
					return data_[index].first;
				}

				QVariant data(int index) const {
					return data_[index].second;
				}

				void add(const QString& name, QVariant v) {
					data_.push_back(std::make_pair(name, v));
				}

			private:
				std::vector<std::pair<QString, QVariant>> data_;
			};
		}
	}
}

