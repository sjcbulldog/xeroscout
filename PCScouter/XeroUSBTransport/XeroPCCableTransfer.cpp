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

#include <Windows.h>
#include "XeroPCCableTransfer.h"
#include "USBDevice.h"
#include <iostream>

namespace xero
{
	namespace device
	{
		namespace usb
		{
			XeroPCCableTransfer::XeroPCCableTransfer(int ba, int da)
			{
				vid_ = default_vid;
				pid_ = default_pid;
				da_ = da;
				ba_ = ba;
				dev_ = nullptr;
			}

			XeroPCCableTransfer::~XeroPCCableTransfer()
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				delete dev;
			}

			bool XeroPCCableTransfer::init()
			{
				USBDevice* dev = new USBDevice(vid_, pid_, ba_, da_);
				if (!dev->init())
				{
					delete dev;
					dev_ = nullptr;
					return false;
				}

				if (!dev->setTimeout(recvPipe, 1))
				{
					delete dev;
					return false;
				}

				if (!dev->setShortPacketTerminate(sendPipe, true))
				{
					delete dev;
					return false;
				}

				if (!dev->setAutoClearStall(sendPipe, true))
				{
					delete dev;
					return false;
				}

				if (!dev->setAutoClearStall(recvPipe, true))
				{
					delete dev;
					return false;
				}

				dev_ = dev ;

				reset();

				return true;
			}

			bool XeroPCCableTransfer::send(const std::vector<uint8_t>& data)
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				return dev->write(sendPipe, data);
			}

			bool XeroPCCableTransfer::receive(std::vector<uint8_t>& data)
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				return dev->read(recvPipe, data);
			}

			bool XeroPCCableTransfer::reset()
			{
				std::vector<uint8_t> data;

				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				if (!dev->abortPipe(sendPipe))
					return false;
				if (!dev->resetPipe(sendPipe))
					return false;
				if (!dev->abortPipe(recvPipe))
					return false;
				if (!dev->resetPipe(recvPipe))
					return false;

				do {
					if (!dev->read(recvPipe, data))
						break;
				} while (data.size() > 0);				

				return true;
			}

			const std::string& XeroPCCableTransfer::description()
			{
				static std::string invalid_("invalid") ;

				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				if (dev == nullptr)
					return invalid_;

				return dev->description();
			}
		}
	}
}
