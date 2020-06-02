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
			}

			void BluetoothClient::connectToServer(const QBluetoothServiceInfo& info)
			{
				socket_ = new QBluetoothSocket();
				connect(socket_, &QBluetoothSocket::connected, this, &BluetoothClient::socketConnected);
				connect(socket_, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error), this, &BluetoothClient::socketConnectError);

				socket_->connectToService(info);
			}

			void BluetoothClient::socketConnected()
			{
				BluetoothTransport* trans = new BluetoothTransport(socket_);
				socket_ = nullptr;

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
					connectToServer(service);
				}
			}
		}
	}
}
