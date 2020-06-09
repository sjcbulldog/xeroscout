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

#pragma once

#include "scouttransport_global.h"
#include "BluetoothTransport.h"
#include <QObject>
#include <QBluetoothServiceDiscoveryAgent>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT BluetoothClient : public QObject
			{
				Q_OBJECT

			public:
				BluetoothClient(int team);
				virtual ~BluetoothClient();

				bool search();

			signals:
				void connected(BluetoothTransport* trans);
				void connectError(const QString& err);

			private:
				void connectToServer(const QBluetoothServiceInfo& info);
				void serviceDiscovered(const QBluetoothServiceInfo& devinfo);
				void timerExpired();
				void socketConnected();
				void socketConnectError(QBluetoothSocket::SocketError error);


			private:
				int team_;
				QTimer* timer_;
				QBluetoothServiceDiscoveryAgent* agent_;
				std::list<QBluetoothServiceInfo> found_;
				QBluetoothSocket* socket_;
				int timeout_;
				QString service_name_;

				QMetaObject::Connection c1_;
				QMetaObject::Connection c2_;
			};
		}
	}
}
