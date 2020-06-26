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
#include "DataSetHighlightRules.h"
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
				ScoutingDataSet(const QString name) {
					name_ = name;
				}

				virtual ~ScoutingDataSet() {
				}

				const QString& name() const {
					return name_;
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
					rules_.clear();
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
					if (col == -1)
						return empty_;
					return get(row, col);
				}

				void set(int row, int col, const QVariant &v) {
					(data_[row])[col] = v;
				}

				bool writeCSV(const QString& filename) const;

				QVariant columnSummary(int col, bool html = false);

				QStringList fieldNames() const {
					QStringList list;
					for (int i = 0; i < columnCount(); i++)
						list.push_back(colHeader(i)->name());

					return list;
				}

				void clearRules() {
					rules_.clear();
				}

				void addRule(std::shared_ptr<const DataSetHighlightRules> rule) {
					rules_.push_back(rule);
				}

				const std::list<std::shared_ptr<const DataSetHighlightRules>>& rules() const {
					return rules_;
				}

				void getCombinations(const QStringList &fields, std::vector<std::vector<QVariant>> &combinations, std::map<int, std::vector<int>> &rowgroups) {
					for (int row = 0; row < rowCount(); row++)
					{
						std::vector<QVariant> rowdata;

						for (const QString& field : fields)
							rowdata.push_back(get(row, field));

						int index = -1;
						for (int comb = 0; comb < combinations.size(); comb++)
						{
							if (combinations[comb] == rowdata)
							{
								index = comb;
								break;
							}
						}

						if (index == -1)
						{
							combinations.push_back(rowdata);
							index = combinations.size() - 1;
							std::vector<int> rows = { row };
							rowgroups.insert_or_assign(index, rows);
						}
						else
						{
							rowgroups[index].push_back(row);
						}
					}
				}

			private:
				QString name_;
				std::vector<std::shared_ptr<FieldDesc>> headers_;
				std::vector<std::vector<QVariant>> data_;
				std::list<std::shared_ptr<const DataSetHighlightRules>> rules_;
				QVariant empty_;
			};

		}
	}
}
