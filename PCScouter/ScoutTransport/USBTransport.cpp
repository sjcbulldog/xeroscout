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
#include <iostream>

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
				packet_no_ = 0;
				waiting_handshake_ = false;
				last_data_.resize(512);
			}

			USBTransport::USBTransport(USBServer *server, xero::device::usb::XeroPCCableTransfer* dev)
			{
				server_ = server;
				usb_ = dev;
				error_ = false;
				running_ = true;
				inited_ = true;
				thread_ = std::thread(&USBTransport::doWork, this);
				packet_no_ = 0;
				waiting_handshake_ = false;
				last_data_.resize(512);
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
				usb_->reset();
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

				auto elapsed = std::chrono::high_resolution_clock::now() - write_time_;
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

				if (waiting_handshake_ && ms.count() > 500) {
					usb_->reset();
					usb_->send(last_data_);
				}

				while (!waiting_handshake_ && getWriteDataSize())
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

				{
					std::lock_guard<std::mutex> guard(write_mutex_);
					remaining = data_to_write_.size();
					if (remaining > USBDataSize)
						remaining = USBDataSize;

					memcpy(last_data_.data() + USBHeaderSize, data_to_write_.data(), remaining);
					last_data_[0] = remaining & 0xFF;
					last_data_[1] = (remaining >> 8) & 0xFF;
					last_data_[2] = 0x88;
					last_data_[3] = 0x88;
					data_to_write_ = data_to_write_.remove(0, remaining);
				}

				std::cout << "USBTransport writing " << last_data_.size() << " bytes" << std::endl;
				write_time_ = std::chrono::high_resolution_clock::now();
				waiting_handshake_ = true;
				if (!usb_->send(last_data_))
					return -1;

				return remaining;
			}

			void USBTransport::appendReadData(const std::vector<uint8_t>& data, int len)
			{
				if (data.size() > 0)
				{
					std::lock_guard<std::mutex> guard(read_mutex_);
					int oldsize = data_read_.size();
					data_read_.resize(static_cast<int>(oldsize + len));
					memcpy(data_read_.data() + oldsize, data.data() + USBHeaderSize, len);
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

						int b1 = (uint8_t)data[0];
						int b2 = (uint8_t)data[1];
						int len = b1 | (b2 << 8);

						b1 = (uint8_t)data[2];
						b2 = (uint8_t)data[3];
						int magic = b1 | (b2 << 8);

						std::cout << "read data, raw length " << len << ", magic " << magic << std::endl;
						
						if (magic == 0x8888) {
							appendReadData(data, len);

							data.resize(4);
							data[0] = 0;
							data[1] = 0;
							data[2] = 0x11;
							data[3] = 0x11;
							usb_->send(data);
						}
						else if (magic == 0x1111) {
							waiting_handshake_ = false;
						}
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
