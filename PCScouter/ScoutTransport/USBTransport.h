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
