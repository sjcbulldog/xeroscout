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