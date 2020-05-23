#pragma once

#include "scouttransport_global.h"
#include "BluetoothBaseTransport.h"
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
			class SCOUTTRANSPORT_EXPORT BluetoothClientTransport : public BluetoothBaseTransport
			{
				Q_OBJECT

			public:
				BluetoothClientTransport();
				virtual ~BluetoothClientTransport();

				bool search();
				bool init(const QBluetoothDeviceInfo& serviceInfo);

				virtual bool write(const QByteArray& data) ;
				virtual QByteArray readAll() ;
				virtual void flush() ;
				virtual void run() ;
				virtual QString type() ;
				virtual QString description() ;

			signals:
				void serverConnected();
				void serverConnectFailed();
				void foundDevice(const QBluetoothDeviceInfo& info);
				void finished();

			private:
				void timerExpired();
				void deviceDiscovered(const QBluetoothDeviceInfo& devinfo);
				void discoveryFinished();

				void readSocket();
				void connected();
				void disconnected();
				void error(QBluetoothSocket::SocketError error);

			private:
				QBluetoothSocket* socket_;
				QBluetoothDeviceDiscoveryAgent* agent_;
				std::list<QBluetoothAddress> found_;
				QTimer* timer_;
				QByteArray data_;
				bool connecting_;
				int timeout_;
			};
		}
	}
}
