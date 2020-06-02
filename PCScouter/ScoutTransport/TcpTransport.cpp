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

				if (server_ == nullptr)
				{
					delete socket_;
				}

				socket_ = nullptr;
			}

			void TcpTransport::close()
			{
				if (server_ != nullptr)
					server_->closingChild(this);
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
				qDebug() << (server_ == nullptr ? QString("client: ") : QString("server: ")) + "TcpTransport: stream disconnected";
				emit transportDisconnected();
			}

			void TcpTransport::errorReceived(QAbstractSocket::SocketError err)
			{
				if (err == QAbstractSocket::SocketError::RemoteHostClosedError)
					emit transportDisconnected();
				else
					emit transportError(toString(err));
			}

			QString TcpTransport::toString(QAbstractSocket::SocketError err)
			{
				QString errstr;

				switch (err) {
				case QAbstractSocket::SocketError::ConnectionRefusedError:
					errstr = "ConnectionRefusedError";
					break;

				case QAbstractSocket::SocketError::RemoteHostClosedError:
					errstr = "RemoteHostClosedError";
					break;

				case QAbstractSocket::SocketError::HostNotFoundError:
					errstr = "HostNotFoundError";
					break;

				case QAbstractSocket::SocketError::SocketAccessError:
					errstr = "SocketAccessError";
					break;

				case QAbstractSocket::SocketError::SocketResourceError:
					errstr = "SocketResourceError";
					break;

				case QAbstractSocket::SocketError::SocketTimeoutError:
					errstr = "SocketTimeoutError";
					break;

				case QAbstractSocket::SocketError::DatagramTooLargeError:
					errstr = "DatagramTooLargeError";
					break;

				case QAbstractSocket::SocketError::NetworkError:
					errstr = "NetworkError";
					break;

				case QAbstractSocket::SocketError::AddressInUseError:
					errstr = "AddressInUseError";
					break;

				case QAbstractSocket::SocketError::SocketAddressNotAvailableError:
					errstr = "SocketAddressNotAvailableError";
					break;

				case QAbstractSocket::SocketError::UnsupportedSocketOperationError:
					errstr = "UnsupportedSocketOperationError";
					break;

				case QAbstractSocket::SocketError::UnfinishedSocketOperationError:
					errstr = "UnfinishedSocketOperationError";
					break;

				case QAbstractSocket::SocketError::ProxyAuthenticationRequiredError:
					errstr = "ProxyAuthenticationRequiredError";
					break;

				case QAbstractSocket::SocketError::SslHandshakeFailedError:
					errstr = "SslHandshakeFailedError";
					break;

				case QAbstractSocket::SocketError::ProxyConnectionRefusedError:
					errstr = "ProxyConnectionRefusedError";
					break;

				case QAbstractSocket::SocketError::ProxyConnectionClosedError:
					errstr = "ProxyConnectionClosedError";
					break;

				case QAbstractSocket::SocketError::ProxyConnectionTimeoutError:
					errstr = "ProxyConnectionTimeoutError";
					break;

				case QAbstractSocket::SocketError::ProxyNotFoundError:
					errstr = "ProxyNotFoundError";
					break;

				case QAbstractSocket::SocketError::ProxyProtocolError:
					errstr = "ProxyProtocolError";
					break;

				case QAbstractSocket::SocketError::OperationError:
					errstr = "OperationError";
					break;

				case QAbstractSocket::SocketError::SslInternalError:
					errstr = "SslInternalError";
					break;

				case QAbstractSocket::SocketError::SslInvalidUserDataError:
					errstr = "SslInvalidUserDataError";
					break;

				case QAbstractSocket::SocketError::TemporaryError:
					errstr = "TemporaryError";
					break;
				}

				return errstr;
			}

			void TcpTransport::dataReceived()
			{
				emit transportDataAvailable();
			}

			QByteArray TcpTransport::readAll()
			{
				QByteArray data = socket_->readAll();

				// qDebug() << (server_ == nullptr ? QString("client: ") : QString("server: ")) + "TcpTransport: received " + QString::number(data.size()) + " bytes";

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

					// qDebug() << (server_ == nullptr ? QString("client: ") : QString("server: ")) + "TcpTransport: wrote " + QString::number(written) + " bytes";

					index += written;

				} while (index != data.size());

				return true;
			}
		}
	}
}
