#include "USBUnitTest.h"
#include <QtCore/QCoreApplication>
#include <iostream>

using namespace xero::scouting::transport;

USBUnitTest::USBUnitTest(QObject *parent) : QObject(parent)
{
	server_ = false;
	usb_server_ = nullptr;
	running_ = false;
}

USBUnitTest::~USBUnitTest()
{
}

void USBUnitTest::runtest()
{
	if (server_)
		runServer();
	else
		runClient();

	QCoreApplication::exit(0);
}

void USBUnitTest::clientDisconnected()
{
	running_ = false;
}

void USBUnitTest::clientConnected(ScoutTransport *trans)
{
	std::cout << "USBUnitTest: server: client connected" << std::endl;
	running_ = true;
	usb_server_transport_ = static_cast<USBTransport*>(trans);
	(void)connect(trans, &ScoutTransport::transportDisconnected, this, &USBUnitTest::clientDisconnected);

	while (running_) {
		QByteArray arr = trans->readAll();
		if (arr.size() == 0)
			continue;

		if (arr.size() != 64) {
			std::cerr << "USBUnitTest: client: invalid data size, got " << arr.size() << ", expected 64" << std::endl;
		}
		else {
			std::cout << "USBUnitTest: client: read 64 bytes" << std::endl;
			trans->write(arr);
		}
	}
}

void USBUnitTest::runServer() 
{
	usb_server_ = new USBServer(nullptr);
	if (!usb_server_->init()) 
	{
		std::cerr << "USBUnitTest: server: transport initialization failed" << std::endl;
		return;
	}
	(void)connect(usb_server_, &ScoutServer::connected, this, &USBUnitTest::clientConnected);

	std::cout << "USBUnitTest: server: hardware " << usb_server_->hwinfo().toStdString() << std::endl;
	std::cout << "USBUnitTest: server: waiting on connection from client" << std::endl;

	while (true) {
		usb_server_->run();
		QCoreApplication::processEvents();
	}
}

void USBUnitTest::runClient()
{
	usb_client_transport_ = new USBTransport();
	if (!usb_client_transport_->init())
	{
		std::cerr << "USBUnitTest: client: transport initialization failed" << std::endl;
		return;
	}
	else {
		std::cout << "USBUnitTest: client: hardware " << usb_client_transport_->description().toStdString() << std::endl;
	}

	QByteArray write;
	write.fill(0x42, 64);

	for(int i = 0 ; i < 100 ; i++) {
		usb_client_transport_->run();
		usb_client_transport_->write(write);
		usb_client_transport_->run();
		while (!usb_client_transport_->hasData()) {
			QByteArray read = usb_client_transport_->readAll();
		}
	}
}