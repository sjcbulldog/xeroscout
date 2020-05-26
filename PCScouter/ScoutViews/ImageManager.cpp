#include "ImageManager.h"

#include <QCoreApplication>
#include <QFile>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ImageManager::ImageManager()
			{
			}

			ImageManager::~ImageManager()
			{
			}

			std::shared_ptr<QImage> ImageManager::get(const QString& str) {
				auto it = images_.find(str);
				if (it != images_.end())
					return it->second;

				QString path = QCoreApplication::applicationDirPath() + "/" + str;
				QFile file(path);
				if (!file.exists())
					return nullptr;

				std::shared_ptr<QImage> image = std::make_shared<QImage>(path);
				if (image->isNull())
					return nullptr;

				images_.insert_or_assign(str, image);
				return image;
			}
		}
	}
}
