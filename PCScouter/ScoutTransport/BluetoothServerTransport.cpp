#include "BluetoothServerTransport.h"
#include <QBluetoothLocalDevice>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{

			BluetoothServerTransport::BluetoothServerTransport()
			{
				server_ = nullptr;
				socket_ = nullptr;
			}

			BluetoothServerTransport::~BluetoothServerTransport()
			{
				if (server_ != nullptr)
					delete server_;

				if (socket_ != nullptr)
					delete socket_;
			}

			bool BluetoothServerTransport::init()
			{
				QBluetoothLocalDevice dev;

				if (!dev.isValid())
				{
					qDebug() << "no bluetooth device found";
					return false;
				}

				dev.powerOn();

				server_ = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
				(void)connect(server_, &QBluetoothServer::newConnection, this, &BluetoothServerTransport::newConnection);

				if (!server_->listen(dev.address()))
				{
					delete server_;
					server_ = nullptr;
					return false;
				}

				QBluetoothServiceInfo::Sequence profileSequence;
				QBluetoothServiceInfo::Sequence classId;
				classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
				classId << QVariant::fromValue(quint16(0x100));
				profileSequence.append(QVariant::fromValue(classId));
				serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,	profileSequence);

				classId.clear();
				classId << QVariant::fromValue(serviceID());
				classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("XeroScout Synchronization"));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, tr("XeroScout Synchronization Server"));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("xeroscout.org"));
				serviceInfo.setServiceUuid(QBluetoothUuid(serviceID()));

				QBluetoothServiceInfo::Sequence publicBrowse;
				publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
				serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList, publicBrowse);

				QBluetoothServiceInfo::Sequence protocolDescriptorList;
				QBluetoothServiceInfo::Sequence protocol;
				protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
				protocolDescriptorList.append(QVariant::fromValue(protocol));
				protocol.clear();
				protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))	<< QVariant::fromValue(quint8(server_->serverPort()));
				protocolDescriptorList.append(QVariant::fromValue(protocol));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,	protocolDescriptorList);

				serviceInfo.registerService(dev.address());

				return true;
			}

			void BluetoothServerTransport::newConnection()
			{
				socket_ = server_->nextPendingConnection();
				if (socket_ == nullptr)
					return;

				data_.clear();

				read_connection_ = connect(socket_, &QBluetoothSocket::readyRead, this, &BluetoothServerTransport::readAvailableData);
				disconnected_connection_ = connect(socket_, &QBluetoothSocket::disconnected, this, &BluetoothServerTransport::disconnected);

				QString name = socket_->peerName();
				if (name.isEmpty())
					name = socket_->peerAddress().toString();
				emit connected(this);
			}

			void BluetoothServerTransport::readAvailableData()
			{
				data_ += socket_->readAll();

				emit transportDataAvailable();
			}

			void BluetoothServerTransport::disconnected()
			{
				emit transportDisconnected();
			}

			bool BluetoothServerTransport::write(const QByteArray& data)
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

			QByteArray BluetoothServerTransport::readAll()
			{
				QByteArray d = data_;
				data_.clear();
				return d;
			}

			void BluetoothServerTransport::flush()
			{
			}

			void BluetoothServerTransport::run()
			{
			}

			QString BluetoothServerTransport::type()
			{
				QString ret = "BluetoothServerTransport";
				return ret;
			}

			QString BluetoothServerTransport::description()
			{
				QString ret = "BluetoothServerTransport";
				return ret;
			}
		}
	}
}
