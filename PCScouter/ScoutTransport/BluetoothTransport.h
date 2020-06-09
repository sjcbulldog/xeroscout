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
