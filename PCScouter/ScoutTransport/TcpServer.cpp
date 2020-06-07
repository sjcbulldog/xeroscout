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
