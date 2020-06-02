#include "BluetoothServer.h"
#include "BluetoothIDS.h"
#include "BluetoothTransport.h"
#include <QBluetoothLocalDevice>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			BluetoothServer::BluetoothServer()
			{
				server_ = nullptr;
			}

			BluetoothServer::~BluetoothServer()
			{
				if (server_ != nullptr)
					delete server_;
			}

			bool BluetoothServer::init()
			{
				QBluetoothLocalDevice dev;

				if (!dev.isValid())
				{
					qDebug() << "no bluetooth device found";
					return false;
				}

				dev.powerOn();

				server_ = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
				(void)connect(server_, &QBluetoothServer::newConnection, this, &BluetoothServer::newConnection);

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
				classId << QVariant::fromValue(BluetoothIDS::serviceID());
				classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("XeroScout Synchronization"));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, tr("XeroScout Synchronization Server"));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("xeroscout.org"));
				serviceInfo.setServiceUuid(QBluetoothUuid(BluetoothIDS::serviceID()));

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

			void BluetoothServer::newConnection()
			{
				QBluetoothSocket *socket = server_->nextPendingConnection();
				if (socket == nullptr)
					return;

				BluetoothTransport* t = new BluetoothTransport(socket);
				emit connected(t);
			}

			void BluetoothServer::run()
			{
			}

			QString BluetoothServer::name()
			{
				QString ret = "BluetoothServer";
				return ret;
			}
		}
	}
}
