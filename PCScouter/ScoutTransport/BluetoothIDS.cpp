#include "BluetoothIDS.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			const QBluetoothUuid BluetoothIDS::service_id_(QString("{a6c79f3f-42d0-4726-92b3-c7d9dd442aa5}"));
			const QString BluetoothIDS::service_name_("XeroTransport");

			BluetoothIDS::BluetoothIDS()
			{
			}

			BluetoothIDS::~BluetoothIDS()
			{
			}
		}
	}
}
