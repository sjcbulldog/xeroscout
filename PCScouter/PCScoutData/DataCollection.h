//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
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

