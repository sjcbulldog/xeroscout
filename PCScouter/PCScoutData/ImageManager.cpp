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

#include "ImageManager.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QImageWriter>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			QStringList ImageManager::server_path_ =
			{
				"$BINDIR",
				"$BINDIR/fields",
				"$BINDIR/fields/images",
				"$APPDIR/images",
			};

			QStringList ImageManager::client_path_ =
			{
				"$APPDIR/images",
			};

			ImageManager::ImageManager(bool server)
			{
				server_ = server;
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

			bool ImageManager::put(const QImage& image, QString& name)
			{
				QString appdir = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).front() + "/images";
				int index = 0;
				QString filename;

				QDir dir(appdir);
				if (!dir.exists(appdir))
				{
					if (!dir.mkpath(appdir))
						return false;
				}

				while (true)
				{
					filename = appdir + "/" + QString::number(index) + ".png";
					QFileInfo info(filename);
					if (!info.exists())
						break;

					index++;
				}

				QFileInfo info(filename);
				name = info.fileName();

				QFile file(filename);
				if (!file.open(QIODevice::WriteOnly))
				{
					return false;
				}
				QImageWriter writer(&file, "png");
				if (!writer.write(image))
				{					
					auto err = writer.error();
					QString errstr = writer.errorString();
					qDebug() << "ImageError: " << errstr;

					return false;
				}

				return true;
			}

			bool ImageManager::put(const QString& str, const QByteArray& data)
			{
				qint64 towrite = data.size();

				QString appdir = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).front() + "/images";
				QDir dir(appdir);
				if (!dir.exists(appdir))
				{
					if (!dir.mkpath(appdir))
						return false;
				}

				appdir += "/" + str;
				QFile file(appdir);
				if (!file.open(QIODevice::WriteOnly))
					return false;

				file.write(data);
				file.close();
				return true;
			}

			std::shared_ptr<QImage> ImageManager::get(const QString& str)
			{
				auto it = images_.find(str);
				if (it != images_.end())
					return it->second;

				QString bindir = QCoreApplication::applicationDirPath();
				QString appdir = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).front();

				QStringList search = (server_ ? server_path_ : client_path_);

				for (const QString& path : search)
				{
					QString p = path;

					p.replace("$APPDIR", appdir);
					p.replace("$BINDIR", bindir);

					p += "/" + str;
					if (QFile::exists(p))
						return loadImage(str, p);
				}
				return nullptr;
			}
		}
	}
}
