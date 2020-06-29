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
				error_connection_ = connect(socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &TcpTransport::errorReceived);
			}

			TcpTransport::TcpTransport(TcpServer *server, QTcpSocket* s)
			{
				server_ = server;
				socket_ = s;

				receive_connection_ = connect(socket_, &QTcpSocket::readyRead, this, &TcpTransport::dataReceived);
				disconnect_connection_ = connect(socket_, &QTcpSocket::disconnected, this, &TcpTransport::streamDisconnected);
				error_connection_ = connect(socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &TcpTransport::errorReceived);
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
