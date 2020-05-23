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
				bool init() override;
				QString name() override { return "TCP"; }

			private:
				void closingChild(TcpTransport*);

			private:
				QTcpServer* server_;
				TcpTransport* child_;
				QObject* parent_;
			};
		}
	}
}
