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
				delete socket_;
				socket_ = nullptr;
				emit transportError("Bluetooth socket error");
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
				socket_->close();
				delete socket_;
				socket_ = nullptr;
			}
		}
	}
}
