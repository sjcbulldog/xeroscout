#include "USBUnitTest.h"
#include <QtCore/QCoreApplication>
#include <iostream>
#include <random>

using namespace xero::scouting::transport;

USBUnitTest::USBUnitTest(QObject *parent) : QObject(parent)
{
	server_ = false;
	usb_server_ = nullptr;
	running_ = false;
	usb_client_transport_ = nullptr;
	usb_server_transport_ = nullptr;
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
	int which = 1;

	std::cout << "USBUnitTest: server: client connected" << std::endl;
	running_ = true;
	usb_server_transport_ = static_cast<USBTransport*>(trans);
	(void)connect(trans, &ScoutTransport::transportDisconnected, this, &USBUnitTest::clientDisconnected);

	while (running_) {
 		QByteArray arr = readpacket(usb_server_transport_);
		trans->write(arr);
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

QByteArray USBUnitTest::readpacket(USBTransport *t)
{
	QByteArray ret, temp;
	int size = -1;

	while (size == -1 || ret.size() < size ) {
		do {
			temp = t->readAll();
		} while (temp.size() == 0);

		ret.append(temp);
		if (size == -1 && ret.size() > 4) {
			int b1 = (uint8_t)ret[0];
			int b2 = (uint8_t)ret[1];
			int b3 = (uint8_t)ret[2];
			int b4 = (uint8_t)ret[3];
			size = b1;
			size |= (b2 << 8);
			size |= (b3 << 16);
			size |= (b4 << 24);
			assert(size >= 0 && size <= MaxPacketSize);
		}
	}

	return ret;
}

void USBUnitTest::runClient()
{
	std::default_random_engine engine;
	std::uniform_int_distribution<int> length(4, MaxPacketSize);
	std::uniform_int_distribution<int> contents(0, 255);

	usb_client_transport_ = new USBTransport();
	if (!usb_client_transport_->init())
	{
		std::cerr << "USBUnitTest: client: transport initialization failed" << std::endl;
		return;
	}
	else {
		std::cout << "USBUnitTest: client: hardware " << usb_client_transport_->description().toStdString() << std::endl;
	}

	QByteArray write, temp, read;
	int which = 1;

	while (true) {
		int size = length(engine);
		write.resize(size);
		for (int i = 4; i < size; i++) {
			write[0] = (size & 0xff);
			write[1] = (size >> 8) & 0xff;
			write[2] = (size >> 16) & 0xff;
			write[3] = (size >> 24) & 0xff;
			write[i] = contents(engine);
		}

		std::cout << "[" << size << "]" << std::flush;
		usb_client_transport_->write(write);

		read = readpacket(usb_client_transport_);

		if (read.size() != write.size()) {
			std::cerr << "USBUnitTest: client: error, returned data length did not match, got " <<
				read.size() << ", send " << write.size() << std::endl;
		}
		else {
			// Sizes match, check contents
			if (write != read) {
				std::cerr << "USBUnitTest: client: error, returned data did not match" << std::endl;
			}
			else {
				std::cout << "USBUnitTest: client: received return packet " << read.size() << std::endl;
			}
		}
	}
}