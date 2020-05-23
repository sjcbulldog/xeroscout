#pragma once

#include "ScoutTransport.h"
#include <QBluetoothUuid>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class BluetoothBaseTransport : public ScoutTransport
			{
			public:
				BluetoothBaseTransport();
				virtual ~BluetoothBaseTransport();

				static const QBluetoothUuid &serviceID() {
					return serviceid;
				}

			private:
				static const QBluetoothUuid serviceid;
			};
		}
	}
}


