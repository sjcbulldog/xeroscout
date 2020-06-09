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

#include <QString>
#include <QStringList>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageItemPublishInfo
			{
			public:
				enum class PublishType
				{
					Average,
					Sum,
				};

			public:
				ImageItemPublishInfo(const QString& tag, PublishType type) {
					tag_ = tag;
					type_ = type;
				}

				void addComponents(const QString& comp) {
					components_.push_back(comp);
				}

				const QStringList& components() const {
					return components_;
				}

				PublishType type() const {
					return type_;
				}

				const QString& tag() const {
					return tag_;
				}

			private:
				QString tag_;
				QStringList components_;
				PublishType type_;
			};
		}
	}
}