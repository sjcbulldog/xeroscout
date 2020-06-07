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


#include "TcpTransport.h"
#include "TcpServer.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			TcpTransport::TcpTransport(QTcpSocket* s)
			{
				socket_ = s;
				server_ = nullptr;

				receive_connection_ = connect(socket_, &QTcpSocket::readyRead, this, &TcpTransport::dataReceived);
				disconnect_connection_ = connect(socket_, &QTcpSocket::disconnected, this, &TcpTransport::streamDisconnected);
				error_connection_ = connect(socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &TcpTransport::errorReceived);
			}

			TcpTransport::TcpTransport(TcpServer *server, QTcpSocket* s)
			{
				server_ = server;
				socket_ = s;

				receive_connection_ = connect(socket_, &QTcpSocket::readyRead, this, &TcpTransport::dataReceived);
				disconnect_connection_ = connect(socket_, &QTcpSocket::disconnected, this, &TcpTransport::streamDisconnected);
				error_connection_ = connect(socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &TcpTransport::errorReceived);
			}

			TcpTransport::~TcpTransport()
			{
				disconnect(receive_connection_);
				disconnect(disconnect_connection_);
				disconnect(error_connection_);

				socket_->close();
				socket_ = nullptr;
			}

			void TcpTransport::close()
			{
				if (server_ != nullptr)
					server_->closing(this);
			}

			QString TcpTransport::description()
			{
				QString ret = "TCP/IP connection";
				ret += ", peer " + socket_->peerName();
				ret += ", port " + QString::number(socket_->peerPort());

				return ret;
			}

			void TcpTransport::run()
			{
			}

			void TcpTransport::flush()
			{
				socket_->waitForBytesWritten(5000);
			}

			void TcpTransport::streamDisconnected()
			{
				emit transportDisconnected();
			}

			void TcpTransport::errorReceived(QAbstractSocket::SocketError err)
			{
				if (err == QAbstractSocket::SocketError::RemoteHostClosedError)
					emit transportDisconnected();
				else
					emit transportError("tcp/ip transport error - " + socket_->errorString());
			}

			void TcpTransport::dataReceived()
			{
				emit transportDataAvailable();
			}

			QByteArray TcpTransport::readAll()
			{
				QByteArray data = socket_->readAll();

				return data;
			}

			bool TcpTransport::write(const QByteArray& data)
			{
				qint64 written;
				qint64 index = 0;

				do {
					written = socket_->write(data.data() + index, data.size() - index);
					if (written == -1)
						return false;

					index += written;

				} while (index != data.size());

				return true;
			}
		}
	}
}
