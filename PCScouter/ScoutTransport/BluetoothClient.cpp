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

#include "BluetoothClient.h"
#include "BluetoothIDS.h"
#include <QBluetoothLocalDevice>
#include <QTimer>
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			BluetoothClient::BluetoothClient(int team)
			{
				team_ = team;
				socket_ = nullptr;
				agent_ = nullptr;
				timer_ = nullptr;
				timeout_ = 15000;
			}

			BluetoothClient::~BluetoothClient()
			{
				if (agent_ != nullptr)
					delete agent_;

				if (timer_ != nullptr)
					delete timer_;
			}

			void BluetoothClient::connectToServer(const QBluetoothServiceInfo& info)
			{
				socket_ = new QBluetoothSocket();
				c1_ = connect(socket_, &QBluetoothSocket::connected, this, &BluetoothClient::socketConnected);
				c2_ = connect(socket_, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error), this, &BluetoothClient::socketConnectError);

				socket_->connectToService(info);
			}

			void BluetoothClient::socketConnected()
			{
				BluetoothTransport* trans = new BluetoothTransport(socket_);
				socket_ = nullptr;
				
				disconnect(c1_);
				disconnect(c2_);
				emit connected(trans);
			}

			void BluetoothClient::socketConnectError(QBluetoothSocket::SocketError error)
			{
				const char* err = "unknown error";

				switch (error)
				{
				case QBluetoothSocket::SocketError::UnknownSocketError:
					break;
				case QBluetoothSocket::SocketError::RemoteHostClosedError:
					err = "remote host closed";
					break;
				case QBluetoothSocket::SocketError::HostNotFoundError:
					err = "host not found";
					break;
				case QBluetoothSocket::SocketError::ServiceNotFoundError:
					err = "service not found";
					break;
				case QBluetoothSocket::SocketError::NetworkError:
					err = "network error";
					break;
				case QBluetoothSocket::SocketError::UnsupportedProtocolError:
					err = "unsupported protocol";
					break;
				case QBluetoothSocket::SocketError::OperationError:
					err = "operation error";
					break;
				}

				disconnect(c1_);
				disconnect(c2_);
				emit connectError(QString("error connecting to xero scout server - ") + err);
			}

			void BluetoothClient::timerExpired()
			{
				timer_->stop();
				delete timer_;
				timer_ = nullptr;

				agent_->stop();
				emit connectError("Timeout connecting to bluetooth xero scout server");
			}

			bool BluetoothClient::search()
			{
				QBluetoothLocalDevice dev;

				if (!dev.isValid())
				{
					qDebug() << "no bluetooth device found";
					return false;
				}

				dev.powerOn();

				agent_ = new QBluetoothServiceDiscoveryAgent(dev.address());
				(void)connect(agent_, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, &BluetoothClient::serviceDiscovered);
				agent_->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

				timer_ = new QTimer();
				timer_->setSingleShot(true);
				timer_->setInterval(1000 * timeout_);
				timer_->start();
				(void)connect(timer_, &QTimer::timeout, this, &BluetoothClient::timerExpired);

				return true;
			}

			void BluetoothClient::serviceDiscovered(const QBluetoothServiceInfo& service)
			{
				if (service.serviceName() == BluetoothIDS::serviceName(team_))
				{
					timer_->stop();
					delete timer_;
					timer_ = nullptr;
					agent_->stop();
					delete agent_;
					agent_ = nullptr;
					connectToServer(service);
				}
			}
		}
	}
}
