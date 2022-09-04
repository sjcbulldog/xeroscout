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

#ifdef _XERO_BLUETOOTH_CLIENT


#include "BluetoothTransport.h"
#include <QBluetoothLocalDevice>
#include <QDebug>
#include <QLoggingCategory>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			BluetoothTransport::BluetoothTransport(QBluetoothSocket *socket) : ScoutTransport(false)
			{
				socket_ = socket;

				connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothTransport::readSocket);
				connect(socket_, &QBluetoothSocket::disconnected, this, &BluetoothTransport::disconnected);
				connect(socket_, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothTransport::error);
			}

			BluetoothTransport::~BluetoothTransport()
			{
			}

			bool BluetoothTransport::init()
			{
				return true;
			}

			void BluetoothTransport::readSocket()
			{
				data_ += socket_->readAll();
				emit transportDataAvailable();
			}

			void BluetoothTransport::disconnected()
			{
				delete socket_;
				socket_ = nullptr;
				emit transportDisconnected();
			}

			void BluetoothTransport::error(QBluetoothSocket::SocketError error)
			{
				if (error == QBluetoothSocket::SocketError::RemoteHostClosedError)
					emit transportDisconnected();
				else
					emit transportError("bluetooth transport error - " + socket_->errorString());
			}

			bool BluetoothTransport::write(const QByteArray& data)
			{
				if (socket_ == nullptr)
					return false;

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

			QByteArray BluetoothTransport::readAll()
			{
				QByteArray d = data_;
				data_.clear();
				return d;
			}

			void BluetoothTransport::flush()
			{
			}

			void BluetoothTransport::run()
			{
			}

			QString BluetoothTransport::type()
			{
				QString ret = "BluetoothTransport";
				return ret;
			}

			QString BluetoothTransport::description()
			{
				QString ret = "BluetoothTransport";
				return ret;
			}

			void BluetoothTransport::close()
			{
				socket_ = nullptr;
			}
		}
	}
}

#endif