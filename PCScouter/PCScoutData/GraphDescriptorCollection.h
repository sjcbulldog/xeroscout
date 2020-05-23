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


