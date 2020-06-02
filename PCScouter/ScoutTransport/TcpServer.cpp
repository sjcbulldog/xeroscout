#include "TcpServer.h"
#include "TcpTransport.h"
#include "ClientServerProtocol.h"

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

			void TcpServer::closingChild(TcpTransport* child)
			{
				delete child_;
				child_ = nullptr;
			}
		}
	}
}
