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
