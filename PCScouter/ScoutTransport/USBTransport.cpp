//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "USBTransport.h"
#include "USBServer.h"
#include <QDebug>

using namespace xero::device::usb;

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			USBTransport::USBTransport()
			{
				server_ = nullptr;
				usb_ = new XeroPCCableTransfer(-1, -1);
				error_ = false;
				running_ = true;
				inited_ = false;
				thread_ = std::thread(&USBTransport::doWork, this);
			}

			USBTransport::USBTransport(USBServer *server, xero::device::usb::XeroPCCableTransfer* dev)
			{
				server_ = server;
				usb_ = dev;
				error_ = false;
				running_ = true;
				inited_ = true;
				thread_ = std::thread(&USBTransport::doWork, this);
			}

			USBTransport::~USBTransport()
			{
				// Tell the worker thread to stop
				running_ = false;

				// Wait for the worker thread to complete
				thread_.join();

				//
				// If we created the USB device, we delete it
				//
				if (server_ == nullptr)
					delete usb_;
			}

			QString USBTransport::description()
			{
				return QString(usb_->description().c_str());
			}

			int USBTransport::getWriteDataSize()
			{
				std::lock_guard<std::mutex> guard(write_mutex_);
				return data_to_write_.size();
			}

			int USBTransport::getReadDataSize()
			{
				std::lock_guard<std::mutex> guard(read_mutex_);
				return data_read_.size();
			}

			void USBTransport::close()
			{
				if (server_ != nullptr)
					server_->closing(this);
			}

			void USBTransport::run()
			{
				int size = getReadDataSize();
				if (size > 0)
				{
					emit transportDataAvailable();
				}
			}

			bool USBTransport::init()
			{
				std::lock_guard<std::mutex> guard(init_mutex_);
				inited_ = usb_->init();
				return inited_;
			}

			void USBTransport::doWork()
			{
				while (running_)
				{
					if (inited_)
					{
						writeData();
						readData();
					}
					else
					{
						std::this_thread::yield();
					}
				}
			}

			void USBTransport::writeData()
			{
				int count;

				while (getWriteDataSize() > 0)
				{
					count = writeDataBlock();
					if (count == -1) {
						error_ = true;
						break;
					}
				}
			}

			int USBTransport::writeDataBlock()
			{
				int remaining;
				std::vector<uint8_t> d;

				{
					std::lock_guard<std::mutex> guard(write_mutex_);
					remaining = data_to_write_.size();
					if (remaining > 512)
						remaining = 512;

					d.resize(remaining);
					memcpy(d.data(), data_to_write_.data(), remaining);
					data_to_write_ = data_to_write_.remove(0, remaining);
				}

				if (!usb_->send(d))
					return -1;

				qDebug() << "Sent " << d.size() << " bytes";
				return remaining;
			}

			void USBTransport::appendReadData(const std::vector<uint8_t>& data)
			{
				if (data.size() > 0)
				{
					std::lock_guard<std::mutex> guard(read_mutex_);
					int oldsize = data_read_.size();
					data_read_.resize(oldsize + data.size());
					memcpy(data_read_.data() + oldsize, data.data(), data.size());
				}
			}

			void USBTransport::readData()
			{
				std::vector<uint8_t> data;

				while (true)
				{
					if (!usb_->receive(data))
					{
						error_ = true;
						break;
					}
					else {
						if (data.size() == 0)
							break;


						qDebug() << "received " << data.size() << " bytes";
						appendReadData(data);
					}
				}
			}

			bool USBTransport::write(const QByteArray& data)
			{
				std::lock_guard<std::mutex> guard(write_mutex_);
				data_to_write_.append(data);

				return true;
			}

			QByteArray USBTransport::readAll()
			{
				std::lock_guard<std::mutex> guard(read_mutex_);

				QByteArray a = data_read_;
				data_read_.clear();

				return a;
			}

			void USBTransport::flush()
			{
				while (getWriteDataSize() > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}

			bool USBTransport::hasData()
			{
				return getReadDataSize() > 0;
			}
		}
	}
}
