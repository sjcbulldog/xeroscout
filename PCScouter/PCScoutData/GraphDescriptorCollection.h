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
#include "GraphDescriptor.h"
#include <QJsonObject>
#include <QJsonArray>
#include <map>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT GraphDescriptorCollection : public std::map<QString, GraphDescriptor>
			{
			public:
				GraphDescriptorCollection();
				virtual ~GraphDescriptorCollection();

				void addGraphDesc(const QString& name, const GraphDescriptor &desc) {
					insert_or_assign(name, desc);
				}

				bool has(const QString& name) const {
					auto it = find(name);
					return it != end();
				}

				GraphDescriptor operator[](const QString& name) const {
					GraphDescriptor empty;

					auto it = find(name);
					if (it == end())
						return empty;

					return it->second;
				}

				bool load(const QJsonArray& obj, QString& err);

				QJsonArray generateJSON() const ;
				
				void update(const GraphDescriptor& desc);

			private:
				bool readDesc(int entry, const QJsonObject& obj, QString& errstr);
				bool readPanes(int entry, GraphDescriptor &desc, const QJsonArray& array, QString& errstr);
				bool isArrayString(const QJsonArray& array);
				bool isArrayDouble(const QJsonArray& array);
			};
		}
	}
}


