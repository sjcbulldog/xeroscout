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
#include "ScoutingDataMap.h"
#include "FieldDesc.h"
#include <QStringList>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT ScoutingDataSet
			{
			public:
				ScoutingDataSet() {
				}

				virtual ~ScoutingDataSet() {
				}

				int columnCount() const {
					return headers_.size();
				}

				int rowCount() const {
					return static_cast<int>(data_.size());
				}

				void clear() {
					headers_.clear();
					data_.clear();
				}

				const std::vector<std::shared_ptr<FieldDesc>> headers() const {
					return headers_;
				}

				const std::shared_ptr<FieldDesc> colHeader(int col) const {
					return headers_[col];
				}

				int getColumnByName(const QString &name) const {
					for (int col = 0; col < columnCount(); col++)
					{
						if (headers_[col]->name() == name)
							return col;
					}

					return -1;
				}

				const std::vector<QVariant>& row(int index) const {
					return data_[index];
				}

				void addHeader(std::shared_ptr<FieldDesc> desc) {
					headers_.push_back(desc);
				}

				void newRow() {
					assert(data_.size() == 0 || data_.back().size() == headers_.size());

					std::vector<QVariant> row;
					data_.push_back(row);
				}

				void addData(const QVariant& v) {
					assert(data_.size() != 0);
					assert(data_.back().size() < headers_.size());

					if (v.isNull())
						data_.back().push_back(QVariant());
					else
						data_.back().push_back(v);
				}

				const QVariant& get(int row, int col) const {
					return (data_[row])[col];
				}

				const QVariant& get(int row, const QString& colname) const {
					int col = getColumnByName(colname);
					return get(row, col);
				}

				void set(int row, int col, const QVariant &v) {
					(data_[row])[col] = v;
				}

				bool writeCSV(const QString& filename) const;

				QVariant columnSummary(int col, bool html = false);

			private:
				std::vector<std::shared_ptr<FieldDesc>> headers_;
				std::vector<std::vector<QVariant>> data_;
			};

		}
	}
}
