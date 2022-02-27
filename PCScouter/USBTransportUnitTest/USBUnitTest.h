#pragma once

#include "USBServer.h"
#include "USBTransport.h"
#include <QObject>

class USBUnitTest : public QObject
{
	Q_OBJECT

public:
	USBUnitTest(QObject *parent);
	~USBUnitTest();

	void setServer() {
		server_ = true;
	}

	void  setClient() {
		server_ = false;
	}

	void runtest();

private:
	void runServer();
	void runClient();

	void clientConnected(xero::scouting::transport::ScoutTransport* trans);
	void clientDisconnected();

	QByteArray readpacket();

	static constexpr const int MaxPacketSize = 32768;

private:
	bool server_;
	bool running_;

	xero::scouting::transport::USBTransport* usb_server_transport_;
	xero::scouting::transport::USBServer *usb_server_;

	xero::scouting::transport::USBTransport* usb_client_transport_;
};
