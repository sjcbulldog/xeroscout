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

			void BluetoothClient::connectToServer(const QBluetoothDeviceInfo& info)
			{
				socket_ = new QBluetoothSocket();
				connect(socket_, &QBluetoothSocket::connected, this, &BluetoothClient::socketConnected);
				connect(socket_, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error), this, &BluetoothClient::socketConnectError);

				socket_->connectToService(info.address(), BluetoothIDS::serviceID());
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

				agent_ = new QBluetoothDeviceDiscoveryAgent(dev.address());
				(void)connect(agent_, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothClient::deviceDiscovered);
				agent_->setInquiryType(QBluetoothDeviceDiscoveryAgent::InquiryType::GeneralUnlimitedInquiry);
				agent_->start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::ClassicMethod);

				timer_ = new QTimer();
				timer_->setSingleShot(true);
				timer_->setInterval(1000 * timeout_);
				timer_->start();
				(void)connect(timer_, &QTimer::timeout, this, &BluetoothClient::timerExpired);

				return true;
			}

			void BluetoothClient::deviceDiscovered(const QBluetoothDeviceInfo& devinfo)
			{
				if (std::find(found_.begin(), found_.end(), devinfo.address()) == found_.end())
				{
					emit foundDevice(devinfo);
					found_.push_back(devinfo.address());
				}
			}

			bool BluetoothClient::createTransport(const QBluetoothDeviceInfo& info)
			{
				agent_->stop();

				socket_ = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
				QBluetoothServiceInfo sinfo;

				sinfo.setDevice(info);
				sinfo.setServiceUuid(BluetoothIDS::serviceID());
				sinfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, QVariant(QBluetoothUuid::SerialPort));
				sinfo.setAttribute(QBluetoothServiceInfo::ServiceId, QVariant(BluetoothIDS::serviceID()));

				QBluetoothServiceInfo::Sequence protdesclist;
				QBluetoothServiceInfo::Sequence protlist;

				protlist << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm));
				protdesclist << protlist;
				sinfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protdesclist);
				assert(sinfo.isComplete());

				socket_->connectToService(info.address(), BluetoothIDS::serviceID());

				return true;
			}
		}
	}
}
