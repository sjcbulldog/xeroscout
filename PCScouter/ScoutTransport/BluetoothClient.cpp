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
			BluetoothClient::BluetoothClient()
			{
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
			}

			void BluetoothClient::timerExpired()
			{
				timer_->stop();
				delete timer_;
				timer_ = nullptr;

				agent_->stop();
				emit discoveryFinished();
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
				auto it = std::find_if(found_.begin(), found_.end(), [service](const QBluetoothServiceInfo& info) { return service.serviceName() == info.serviceName();  });
				if (it == found_.end())
				{
					emit foundService(service);
					found_.push_back(service);
				}

				qDebug() << "Discovered service on"
					<< service.device().name() << service.device().address().toString();
				qDebug() << "\tService name:" << service.serviceName();
				qDebug() << "\tDescription:"
					<< service.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
				qDebug() << "\tProvider:"
					<< service.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
				qDebug() << "\tL2CAP protocol service multiplexer:"
					<< service.protocolServiceMultiplexer();
				qDebug() << "\tRFCOMM server channel:" << service.serverChannel();
			}
		}
	}
}
