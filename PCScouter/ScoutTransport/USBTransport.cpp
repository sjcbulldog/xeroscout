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
					if (remaining > MaxUSBPacketSize)
						remaining = MaxUSBPacketSize;

					d.resize(remaining);
					memcpy(d.data(), data_to_write_.data(), remaining);
					data_to_write_ = data_to_write_.remove(0, remaining);
				}

				if (!usb_->send(d))
					return -1;

				return remaining;
			}

			void USBTransport::appendReadData(const std::vector<uint8_t>& data)
			{
				if (data.size() > 0)
				{
					std::lock_guard<std::mutex> guard(read_mutex_);
					int oldsize = data_read_.size();
					data_read_.resize(static_cast<int>(oldsize + data.size()));
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
