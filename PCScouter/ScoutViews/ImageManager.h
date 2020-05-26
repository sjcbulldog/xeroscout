#pragma once

#include "ImageSupplier.h"
#include <QString>
#include <QImage>
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

			public:
				std::map<QString, std::shared_ptr<QImage>> images_;
			};
		}
	}
}
