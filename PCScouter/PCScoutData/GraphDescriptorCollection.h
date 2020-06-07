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


