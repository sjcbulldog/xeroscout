//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
