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

					strm << '"' << headers_[i] << '"';
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

			bool ScoutingDataSet::isColumnBool(int col) const
			{
				for (int row = 0; row < rowCount(); row++)
				{
					const QVariant& v = get(row, col);
					auto t = v.type();
					if (t != QVariant::Type::Int && t != QVariant::Type::LongLong && t != QVariant::UInt && t != QVariant::ULongLong)
						return false;

					if (v.toInt() != 0 && v.toInt() != 1)
						return false;
				}

				return true;
			}
		}
	}
}
