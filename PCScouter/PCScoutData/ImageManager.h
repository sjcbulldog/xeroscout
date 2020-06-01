#pragma once

#include "pcscoutdata_global.h"
#include "ImageSupplier.h"
#include <QString>
#include <QImage>
#include <QFile>
#include <memory>
#include <map>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT ImageManager : public xero::scouting::datamodel::ImageSupplier
			{
			public:
				ImageManager(bool server);
				virtual ~ImageManager();

				std::shared_ptr<QImage> get(const QString& tag);
				bool put(const QString& tag, const QByteArray& data);

			private:
				std::shared_ptr<QImage> loadImage(const QString &str, const QString& file);

			private:
				std::map<QString, std::shared_ptr<QImage>> images_;

				static QStringList server_path_;
				static QStringList client_path_;

				bool server_;
			};
		}
	}
}
