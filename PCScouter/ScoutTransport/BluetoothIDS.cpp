//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

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
