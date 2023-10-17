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

				bool init(std::stringstream &messages) override { return true; }
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
