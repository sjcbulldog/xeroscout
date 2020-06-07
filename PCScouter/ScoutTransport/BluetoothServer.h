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
				BluetoothServer(int team, QObject *parent);
				virtual ~BluetoothServer();

				virtual bool init() ;
				virtual void run() ;
				virtual QString name() ;
				virtual QString hwinfo();

			private:
				void newConnection();

			private:
				QBluetoothServer* server_;
				QBluetoothServiceInfo serviceInfo;
				int team_;
			};
		}
	}
}

