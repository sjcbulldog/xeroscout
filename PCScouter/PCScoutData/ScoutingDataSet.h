//
// Copyright 2020 by Jack W. (Butch) Griffin
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
					types_.clear();
				}

				const QStringList& headers() const {
					return headers_;
				}

				const QString& colHeader(int col) const {
					return headers_[col];
				}

				const std::vector<QVariant::Type>& types() const {
					return types_;
				}

				QVariant::Type colType(int col) const {
					return types_[col];
				}

				const std::vector<QVariant>& row(int index) const {
					return data_[index];
				}

				void addHeader(const QString& name, QVariant::Type t) {
					headers_.push_back(name);
					types_.push_back(t);
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

				bool writeCSV(const QString& filename) const;

				bool isColumnBool(int col) const;

			private:
				QStringList headers_;
				std::vector<QVariant::Type> types_;
				std::vector<std::vector<QVariant>> data_;
			};

		}
	}
}
