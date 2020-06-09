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

#include "TcpServer.h"
#include "TcpTransport.h"
#include "ClientServerProtocol.h"
#include <QNetworkInterface>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			TcpServer::TcpServer(QObject *p) : ScoutServer(p)
			{
				parent_ = p;
				child_ = nullptr;
				server_ = nullptr;
			}

			TcpServer::~TcpServer()
			{
			}

			QString TcpServer::hwinfo()
			{
				QString ret;

				auto list = QNetworkInterface::allAddresses();
				for (const auto& addr : list)
				{
					if (addr.isLoopback() || addr.isNull() || addr.isLinkLocal())
						continue;

					if (ret.length() > 0)
						ret += ", ";

					ret += addr.toString();
				}

				return ret;
			}

			void TcpServer::closing(TcpTransport* trans)
			{
				assert(child_ == trans);
				child_ = nullptr;
			}

			void TcpServer::run()
			{
				if (child_ == nullptr && server_->hasPendingConnections())
				{
					QTcpSocket* socket = server_->nextPendingConnection();
					child_ = new TcpTransport(this, socket);

					emit connected(child_);
				}
			}

			bool TcpServer::init()
			{
				server_ = new QTcpServer(parent_);

				if (!server_->listen(QHostAddress::AnyIPv4, ClientServerProtocol::ScoutStreamPort))
					return false;

				return true;
			}
		}
	}
}
