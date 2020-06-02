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
				void closingChild(USBTransport*);

			private:
				bool inited_;
				xero::device::usb::XeroPCCableTransfer* usb_;

				USBTransport* future_child_;
				USBTransport* active_child_;
				USBTransport* dead_child_;

				QObject* parent_;
			};
		}
	}
}
