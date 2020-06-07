//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

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

				bool has(const QString& name) const {
					for (int i = 0; i < count() ; i++)
					{
						if (tag(i) == name)
							return true;
					}

					return false;
				}

				QVariant dataByName(const QString& name) const {
					QVariant v;

					for (int i = 0; i < count(); i++)
					{
						if (tag(i) == name)
						{
							v = data(i);
							break;
						}
					}

					return v;
				}

			private:
				std::vector<std::pair<QString, QVariant>> data_;
			};
		}
	}
}

