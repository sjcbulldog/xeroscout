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
