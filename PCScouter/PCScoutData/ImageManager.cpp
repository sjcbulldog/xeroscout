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

#include "ImageManager.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

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
