#pragma once

#include "pcscoutdata_global.h"
#include <QJsonArray>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT PickListTranslator
			{
			public:
				PickListTranslator();
				virtual ~PickListTranslator();

				bool load(const QString& filename);
				bool load(const QJsonArray& obj);
				const QString& error() const { return error_; }

				const std::list<std::pair<QString, QString>>& fields() const {
					return fields_;
				}

				QJsonArray toJSON() const;

			private:
				QString error_;
				std::list<std::pair<QString, QString>> fields_;
			};
		}
	}
}
