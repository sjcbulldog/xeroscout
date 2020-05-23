#include "BluetoothBaseTransport.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			const QBluetoothUuid BluetoothBaseTransport::serviceid(QString("{a6c79f3f-42d0-4726-92b3-c7d9dd442aa5}"));

			BluetoothBaseTransport::BluetoothBaseTransport()
			{
			}

			BluetoothBaseTransport::~BluetoothBaseTransport()
			{
			}
		}
	}
}
