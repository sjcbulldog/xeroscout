#pragma once

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
		namespace views
		{
			class ImageManager : public xero::scouting::datamodel::ImageSupplier
			{
			public:
				ImageManager();
				virtual ~ImageManager();

				std::shared_ptr<QImage> get(const QString& tag);

			private:
				std::shared_ptr<QImage> loadImage(const QString &str, const QString& file);

			private:
				std::map<QString, std::shared_ptr<QImage>> images_;
			};
		}
	}
}
