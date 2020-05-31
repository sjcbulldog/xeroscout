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

			std::shared_ptr<QImage> ImageManager::loadImage(const QString &str, const QString& file)
			{
				std::shared_ptr<QImage> image = std::make_shared<QImage>(file);
				if (image->isNull())
					return nullptr;

				images_.insert_or_assign(str, image);
				return image;
			}

			std::shared_ptr<QImage> ImageManager::get(const QString& str) 
			{
				auto it = images_.find(str);
				if (it != images_.end())
					return it->second;

				QString path = QCoreApplication::applicationDirPath() + "/" + str;
				if (QFile::exists(path))
					return loadImage(str, path);

				path = QCoreApplication::applicationDirPath() + "/fields/" + str;
				if (QFile::exists(path))
					return loadImage(str, path);

				path = QCoreApplication::applicationDirPath() + "/images/" + str;
				if (QFile::exists(path))
					return loadImage(str, path);

				return nullptr;
			}
		}
	}
}
