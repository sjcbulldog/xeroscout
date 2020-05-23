#include "BluetoothClientTransport.h"
#include <QBluetoothLocalDevice>
#include <QDebug>
#include <QLoggingCategory>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			BluetoothClientTransport::BluetoothClientTransport()
			{
				agent_ = nullptr;
				timer_ = nullptr;
				socket_ = nullptr;
				connecting_ = false;
				timeout_ = 45;
			}

			BluetoothClientTransport::~BluetoothClientTransport()
			{
				if (timer_ != nullptr)
					delete timer_;

				if (agent_ != nullptr)
					delete agent_;
			}

			bool BluetoothClientTransport::init(const QBluetoothDeviceInfo& info)
			{
				connecting_ = true;
				agent_->stop();

				socket_ = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
				QBluetoothServiceInfo sinfo;

				sinfo.setDevice(info);
				sinfo.setServiceUuid(serviceID());
				sinfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, QVariant(QBluetoothUuid::SerialPort));
				sinfo.setAttribute(QBluetoothServiceInfo::ServiceId, QVariant(serviceID()));

				QBluetoothServiceInfo::Sequence protdesclist;
				QBluetoothServiceInfo::Sequence protlist;

				protlist << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm));
				protdesclist << protlist;
				sinfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protdesclist);
				assert(sinfo.isComplete());

				socket_->connectToService(info.address(), serviceID());

				connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothClientTransport::readSocket);
				connect(socket_, &QBluetoothSocket::connected, this, QOverload<>::of(&BluetoothClientTransport::connected));
				connect(socket_, &QBluetoothSocket::disconnected, this, &BluetoothClientTransport::disconnected);
				connect(socket_, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothClientTransport::error);

				return true;
			}

			void BluetoothClientTransport::deviceDiscovered(const QBluetoothDeviceInfo& devinfo)
			{
				if (std::find(found_.begin(), found_.end(), devinfo.address()) == found_.end())
				{
					emit foundDevice(devinfo);
					found_.push_back(devinfo.address());
				}
			}

			bool BluetoothClientTransport::search()
			{
				QBluetoothLocalDevice dev;

				if (!dev.isValid())
				{
					qDebug() << "no bluetooth device found";
					return false;
				}

				dev.powerOn();

				agent_ = new QBluetoothDeviceDiscoveryAgent(dev.address());
				(void)connect(agent_, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothClientTransport::deviceDiscovered);
				agent_->setInquiryType(QBluetoothDeviceDiscoveryAgent::InquiryType::GeneralUnlimitedInquiry);
				agent_->start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::ClassicMethod);

				timer_ = new QTimer();
				timer_->setSingleShot(true);
				timer_->setInterval(1000 * timeout_);
				timer_->start();
				(void)connect(timer_, &QTimer::timeout, this, &BluetoothClientTransport::timerExpired);

				return true;
			}

			void BluetoothClientTransport::timerExpired()
			{
				timer_->stop();
				delete timer_;
				timer_ = nullptr;

				agent_->stop();
				emit discoveryFinished();
			}

			void BluetoothClientTransport::discoveryFinished()
			{
				emit finished();
			}

			void BluetoothClientTransport::readSocket()
			{
				data_ += socket_->readAll();
				emit transportDataAvailable();
			}

			void BluetoothClientTransport::connected()
			{
				connecting_ = false;
				emit serverConnected();
			}

			void BluetoothClientTransport::disconnected()
			{
				delete socket_;
				socket_ = nullptr;
				emit transportDisconnected();
			}

			void BluetoothClientTransport::error(QBluetoothSocket::SocketError error)
			{
				delete socket_;
				socket_ = nullptr;

				if (connecting_)
					emit serverConnectFailed();
				else
					emit transportError("Bluetooth socket error");
			}

			bool BluetoothClientTransport::write(const QByteArray& data)
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

			QByteArray BluetoothClientTransport::readAll()
			{
				QByteArray d = data_;
				data_.clear();
				return d;
			}

			void BluetoothClientTransport::flush()
			{
			}

			void BluetoothClientTransport::run()
			{
			}

			QString BluetoothClientTransport::type()
			{
				QString ret = "BluetoothClientTransport";
				return ret;
			}

			QString BluetoothClientTransport::description()
			{
				QString ret = "BluetoothClientTransport";
				return ret;
			}
		}
	}
}
