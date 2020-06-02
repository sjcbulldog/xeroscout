#pragma once

#include "ScoutTransport.h"
#include <QBluetoothUuid>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class BluetoothIDS
			{
			private:
				BluetoothIDS();
				virtual ~BluetoothIDS();

			public:
				static const QBluetoothUuid &serviceID() {
					return serviceid;
				}

			private:
				static const QBluetoothUuid serviceid;
			};
		}
	}
}


