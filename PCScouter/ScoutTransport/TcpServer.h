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

#include "ScoutServer.h"
#include <QTcpServer>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class TcpTransport;

			class SCOUTTRANSPORT_EXPORT TcpServer : public ScoutServer
			{
				friend TcpTransport;

			public:
				TcpServer(QObject *p);
				virtual ~TcpServer();

				void run() override;
				bool init(std::stringstream &messages) override;
				QString name() override { return "TCP"; }
				virtual QString hwinfo();

			private:
				void closing(TcpTransport*);

			private:
				QTcpServer* server_;
				TcpTransport* child_;
				QObject* parent_;
			};
		}
	}
}
