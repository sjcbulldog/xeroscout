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

#include "XeroPCCableTransfer.h"
#include "ScoutServer.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class USBTransport;

			class SCOUTTRANSPORT_EXPORT USBServer : public ScoutServer
			{
				friend class USBTransport;

			public:
				USBServer(QObject *parent);
				virtual ~USBServer();

				void run() override;
				bool init() override;
				QString name() override { return "USB"; }
				virtual QString hwinfo();

			private:
				void closing(USBTransport*);

			private:
				bool inited_;
				xero::device::usb::XeroPCCableTransfer* usb_;

				USBTransport* future_child_;
				USBTransport* active_child_;
				USBTransport* dead_child_;
			};
		}
	}
}
