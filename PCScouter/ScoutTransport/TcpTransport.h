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
#include "ScoutTransport.h"
#include <QTcpSocket>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class TcpServer;

			class SCOUTTRANSPORT_EXPORT TcpTransport : public ScoutTransport
			{
			public:
				TcpTransport(TcpServer *server, QTcpSocket* s);
				TcpTransport(QTcpSocket* s);
				virtual ~TcpTransport();

				QString toString(QAbstractSocket::SocketError err);

				bool init() override { return true; }
				bool write(const QByteArray& data) override ;
				QByteArray readAll() override;
				void flush() override;
				void run() override;
				QString type() { return QString("network"); }
				QString description() override;
				void close() override;

			private:
				void dataReceived();
				void streamDisconnected();
				void errorReceived(QAbstractSocket::SocketError err);

			private:
				QTcpSocket* socket_;
				QMetaObject::Connection receive_connection_;
				QMetaObject::Connection disconnect_connection_;
				QMetaObject::Connection error_connection_;

				TcpServer* server_;
			};
		}
	}
}
