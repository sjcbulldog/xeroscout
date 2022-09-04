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


#include "BluetoothServer.h"
#include "BluetoothIDS.h"
#include "BluetoothTransport.h"
#include <QBluetoothLocalDevice>
#include <QBluetoothAddress>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			BluetoothServer::BluetoothServer(int team, QObject *parent) : ScoutServer(parent)
			{
				team_ = team;
				server_ = nullptr;
			}

			BluetoothServer::~BluetoothServer()
			{
				if (server_ != nullptr)
					delete server_;
			}

			QString BluetoothServer::hwinfo()
			{
				return server_->serverAddress().toString();
			}

			bool BluetoothServer::init()
			{
				QBluetoothAddress addr;

				server_ = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
				(void)connect(server_, &QBluetoothServer::newConnection, this, &BluetoothServer::newConnection);

				if (!server_->listen(addr))
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
				serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList, profileSequence);

				classId.clear();
				classId << QVariant::fromValue(QBluetoothUuid(BluetoothIDS::serviceID()));
				classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

				serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, BluetoothIDS::serviceName(team_));
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

				serviceInfo.registerService(addr);

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

#endif
