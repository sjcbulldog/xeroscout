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

#include "ScoutingDataSet.h"
#include <QFile>
#include <QTextStream>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			bool ScoutingDataSet::writeCSV(const QString& filename) const
			{
				QFile file(filename);
				if (!file.open(QIODevice::WriteOnly))
					return false;

				QTextStream strm(&file);

				for (int i = 0; i < headers_.size(); i++) {
					if (i != 0)
						strm << ',';

					strm << '"' << headers_[i]->name() << '"';
				}
				strm << '\n';

				for (int row = 0; row < rowCount(); row++) {
					for (int col = 0; col < columnCount(); col++) {
						const QVariant& v = get(row, col);
						if (col != 0)
							strm << ',';

						if (v.isValid()) {
							if (v.type() == QVariant::Type::String) {
								strm << '"' << v.toString() << '"';
							}
							if (v.type() == QVariant::Type::Int) {
								strm << v.toString();
							}
							if (v.type() == QVariant::Type::Double) {
								strm << v.toString();
							}
							if (v.type() == QVariant::Type::Bool) {
								strm << v.toString();
							}
						}
						else {
							//
							// Invalid entries are just missing data, leave blank
							//
						}
					}
					strm << '\n';
				}

				return true;
			}

			QVariant ScoutingDataSet::columnSummary(int col, bool html)
			{
				QVariant ret;

				auto hdr = colHeader(col);
				if (hdr->type() == FieldDesc::Type::Integer || hdr->type() == FieldDesc::Type::Double)
				{
					double total = 0.0;
					double count = 0.0;
					for (int row = 0; row < rowCount(); row++)
					{
						QVariant v = get(row, col);
						if (v.isValid())
						{
							total += get(row, col).toDouble();
							count += 1.0;
						}
					}

					total /= count;
					ret = QVariant(total);
				}
				else if (hdr->type() == FieldDesc::Type::Boolean)
				{
					double trueval = 0;
					double count = 0.0;
					for (int row = 0; row < rowCount(); row++)
					{
						QVariant v = get(row, col);
						if (v.isValid())
						{
							if (v.toBool())
								trueval += 1.0;
							count += 1.0;
						}
					}

					ret = QVariant(trueval / count);
				}
				else if (hdr->type() == FieldDesc::Type::StringChoice || hdr->type() == FieldDesc::Type::String)
				{
					std::map<QString, int> strmap;

					double count = 0.0;
					for (int row = 0; row < rowCount(); row++)
					{
						QVariant v = get(row, col);
						if (v.isValid()) {
							count += 1.0;
							int cnt = 0;
							auto it = strmap.find(v.toString());
							if (it != strmap.end())
								cnt = it->second;

							cnt++;
							strmap.insert_or_assign(v.toString(), cnt);
						}
					}

					QString txt;
					bool first = true;
					for (auto pair : strmap)
					{
						if (!first)
						{
							if (html)
								txt += "<br/>";
							else
								txt += ",";
						}

						txt += pair.first + " ";
						double pcnt = (double)pair.second / count * 100.0;
						txt += "(" + QString::number(pcnt, 'f', 2) + ")";

						first = false;
					}

					ret = QVariant(txt);
				}

				return ret;
			}

		}
	}
}
