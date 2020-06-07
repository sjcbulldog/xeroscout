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
#include "ScoutTransport.h"
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QTimer>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT BluetoothTransport : public ScoutTransport
			{
				Q_OBJECT

			public:
				BluetoothTransport(QBluetoothSocket* socket);
				virtual ~BluetoothTransport();

				virtual bool write(const QByteArray& data) ;
				virtual QByteArray readAll() ;
				virtual void flush() ;
				virtual void run() ;
				virtual QString type() ;
				virtual QString description() ;
				virtual bool init();
				virtual void close();

			signals:
				void finished();

			private:
				void readSocket();
				void disconnected();
				void error(QBluetoothSocket::SocketError error);

			private:
				QBluetoothSocket* socket_;
				QByteArray data_;
			};
		}
	}
}
