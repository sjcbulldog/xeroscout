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
				BluetoothClient();
				virtual ~BluetoothClient();

				bool search();
				void connectToServer(const QBluetoothServiceInfo& info);

			signals:
				void discoveryFinished();
				void foundService(const QBluetoothServiceInfo& info);
				void connected(BluetoothTransport* trans);
				void connectError(const QString& err);

			private:
				void serviceDiscovered(const QBluetoothServiceInfo& devinfo);
				void timerExpired();
				void socketConnected();
				void socketConnectError(QBluetoothSocket::SocketError error);


			private:
				QTimer* timer_;
				QBluetoothServiceDiscoveryAgent* agent_;
				std::list<QBluetoothServiceInfo> found_;
				QBluetoothSocket* socket_;
				int timeout_;
			};
		}
	}
}
