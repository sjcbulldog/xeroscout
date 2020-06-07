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
			BluetoothTransport::BluetoothTransport(QBluetoothSocket *socket)
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
