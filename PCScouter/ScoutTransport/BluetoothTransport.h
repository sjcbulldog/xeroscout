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
				BluetoothTransport(QBluetoothSocket *socket);
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
