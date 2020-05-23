#pragma once

#include "scouttransport_global.h"
#include "BluetoothBaseTransport.h"
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QBluetoothServiceInfo>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT BluetoothServerTransport : public BluetoothBaseTransport
			{
				Q_OBJECT

			public:
				BluetoothServerTransport();
				virtual ~BluetoothServerTransport();

				bool init();

				virtual bool write(const QByteArray& data);
				virtual QByteArray readAll();
				virtual void flush();
				virtual void run();
				virtual QString type();
				virtual QString description();
				virtual bool canTakeConnections() { return true; }

			private:
				void newConnection();
				void readAvailableData();
				void disconnected();

			private:
				QBluetoothServer* server_;
				QBluetoothSocket* socket_;
				QBluetoothServiceInfo serviceInfo;

				QByteArray data_;

				QMetaObject::Connection read_connection_;
				QMetaObject::Connection disconnected_connection_;
			};
		}
	}
}

