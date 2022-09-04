//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#ifdef _XERO_BLUETOOTH_CLIENT


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

#endif