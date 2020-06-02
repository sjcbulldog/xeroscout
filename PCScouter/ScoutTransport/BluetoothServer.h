#pragma once

#include "scouttransport_global.h"
#include "ScoutServer.h"
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QBluetoothServiceInfo>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT BluetoothServer : public ScoutServer
			{
				Q_OBJECT

			public:
				BluetoothServer();
				virtual ~BluetoothServer();


				virtual bool init() ;
				virtual void run() ;
				virtual QString name() ;

			private:
				void newConnection();

			private:
				QBluetoothServer* server_;
				QBluetoothServiceInfo serviceInfo;
			};
		}
	}
}

