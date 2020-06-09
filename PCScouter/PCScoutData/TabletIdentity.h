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
