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

