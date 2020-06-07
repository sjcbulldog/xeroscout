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
