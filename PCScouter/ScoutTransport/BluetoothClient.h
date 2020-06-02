#pragma once

#include "scouttransport_global.h"
#include "BluetoothTransport.h"
#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>

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
				void connectToServer(const QBluetoothDeviceInfo& info);

			signals:
				void discoveryFinished();
				void foundDevice(const QBluetoothDeviceInfo& info);
				void connected(BluetoothTransport* trans);
				void connectError(const QString& err);

			private:
				bool createTransport(const QBluetoothDeviceInfo& info);
				void deviceDiscovered(const QBluetoothDeviceInfo& devinfo);
				void timerExpired();
				void socketConnected();
				void socketConnectError(QBluetoothSocket::SocketError error);


			private:
				QTimer* timer_;
				QBluetoothDeviceDiscoveryAgent* agent_;
				std::list<QBluetoothAddress> found_;
				QBluetoothSocket* socket_;
				int timeout_;
			};
		}
	}
}
