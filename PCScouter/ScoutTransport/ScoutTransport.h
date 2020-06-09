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

#include "scouttransport_global.h"
#include <QObject>
#include <QByteArray>
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT ScoutTransport : public QObject
			{
				Q_OBJECT;

			public:
				ScoutTransport() {
				}

				virtual ~ScoutTransport() {
				}

				virtual bool init() = 0;
				virtual bool write(const QByteArray& data) = 0;
				virtual QByteArray readAll() = 0;
				virtual void flush() = 0;
				virtual void run() = 0;
				virtual QString type() = 0;
				virtual QString description() = 0;
				virtual void close() = 0;

				void setServer(bool v) { server_ = v; }
				bool isServer() const { return server_; }

			signals:
				void transportDataAvailable();
				void transportDisconnected();
				void transportError(const QString& errmsg);

			private:
				bool server_;
			};
		}
	}
}

