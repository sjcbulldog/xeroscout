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

#include "ScoutTransport.h"
#include "XeroPCCableTransfer.h"
#include <QDebug>
#include <thread>
#include <mutex>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class USBServer;

			class SCOUTTRANSPORT_EXPORT USBTransport : public ScoutTransport
			{
			public:
				USBTransport();
				USBTransport(USBServer *server, xero::device::usb::XeroPCCableTransfer* dev);

				virtual ~USBTransport();

				bool init() override ;
				bool write(const QByteArray& data) override;
				QByteArray readAll() override;
				void flush() override;
				void run() override;
				QString type() override { return QString("USB"); }
				QString description() override;
				void close() override;

				bool hasData();

				bool hasError() {
					return error_;
				}

			private:
				void doWork();
				void writeData();
				void readData();

				void appendReadData(const std::vector<uint8_t>& data);
				int writeDataBlock();

				int getWriteDataSize();
				int getReadDataSize();

			private:
				std::thread thread_;

				std::mutex read_mutex_;
				std::mutex write_mutex_;
				std::mutex init_mutex_;

				QByteArray data_read_;
				QByteArray data_to_write_;

				bool running_;
				bool data_flowing_;
				bool error_;
				bool debug_;
				bool inited_;

				xero::device::usb::XeroPCCableTransfer* usb_;
				USBServer* server_;
			};
		}
	}
}
