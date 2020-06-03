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
