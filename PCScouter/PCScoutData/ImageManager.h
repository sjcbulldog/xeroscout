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
				bool put(const QImage& image, QString& name);

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
