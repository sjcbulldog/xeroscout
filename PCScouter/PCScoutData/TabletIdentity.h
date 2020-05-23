//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include <QUuid>
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class TabletIdentity
			{
			public:
				TabletIdentity(const QString& name, const QUuid& uid) {
					name_ = name;
					uid_ = uid;
				}

				TabletIdentity(const QUuid& uid) {
					uid_ = uid;
				}

				TabletIdentity() {
					uid_ = QUuid::createUuid();
				}

				TabletIdentity(const QString& name) {
					QString bogus = "";
					name_ = name;
					uid_ = QUuid::fromString(bogus);
				}

				virtual ~TabletIdentity() {
				}

				const QUuid& uid() const {
					return uid_;
				}

				const QString& name() const {
					return name_;
				}

			private:
				QString name_;
				QUuid uid_;
			};
		}
	}
}
