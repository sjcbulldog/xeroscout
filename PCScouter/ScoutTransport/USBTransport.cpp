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
			USBTransport::USBTransport() : ScoutTransport(false)
			{
				server_ = nullptr;
				usb_ = new XeroPCCableTransfer(-1, -1);
				error_ = false;
				running_ = true;
				inited_ = false;
				thread_ = std::thread(&USBTransport::doWork, this);
				read_packet_no_ = 0;
				write_packet_no_ = 0;
				waiting_handshake_ = false;
				last_data_.resize(512);
			}

			USBTransport::USBTransport(USBServer *server) : ScoutTransport(true)
			{
				server_ = server;
				usb_ = new XeroPCCableTransfer(-1, -1);
				error_ = false;
				running_ = true;
				inited_ = false;
				thread_ = std::thread(&USBTransport::doWork, this);
				read_packet_no_ = 0;
				write_packet_no_ = 0;
				waiting_handshake_ = false;
				last_data_.resize(512);
			}

			USBTransport::~USBTransport()
			{
				// Tell the worker thread to stop
				running_ = false;

				// Wait for the worker thread to complete
				thread_.join();

				delete usb_;
			}

			QString USBTransport::description()
			{
				return QString(usb_->description().c_str());
			}

			int USBTransport::getWriteDataSize()
			{
				std::lock_guard<std::mutex> guard(write_mutex_);
				int size = 0;
				for (const QByteArray& data : data_to_write_) {
					size += data.size();
				}
				return size;
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

			bool USBTransport::reset()
			{
				std::lock_guard<std::mutex> guard(init_mutex_);

				usb_->reset();
				return true;
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

					std::this_thread::yield();
				}
			}

			void USBTransport::writeData()
			{
				int count;

				auto elapsed = std::chrono::high_resolution_clock::now() - write_time_;
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

				if (waiting_handshake_ && ms.count() > 1000) {
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
					if (data_to_write_.size() > 0) {
						QByteArray& data = data_to_write_.front();
						remaining = data.size();
						if (remaining > USBDataSize)
							remaining = USBDataSize;

						memcpy(last_data_.data() + USBHeaderSize, data.data(), remaining);
						last_data_[0] = remaining & 0xFF;
						last_data_[1] = (remaining >> 8) & 0xFF;
						last_data_[2] = 0x88;
						last_data_[3] = 0x88;

						std::cout << "Wrote USB data " << remaining << " bytes" << std::endl;

						data = data.remove(0, remaining);
						if (data.size() == 0) {
							std::cout << "    Removed write request" << std::endl;
							data_to_write_.remove(0);
						}

					}
				}

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
							std::cout << "    wrote handshake" << std::endl;
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
				std::cout << "USBTransport::write called - new entry " << data.size() << " bytes, ";
				std::cout << data_to_write_.size() << " entries in the queue" << std::endl;
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
