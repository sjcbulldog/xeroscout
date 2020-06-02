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
